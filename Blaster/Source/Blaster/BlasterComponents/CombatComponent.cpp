// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, bIsAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(Character && Character->IsLocallyControlled())
	{
		// Hit Target
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		
		// Draw Crosshairs
		SetHUDCrosshairs(DeltaTime);

		// 조준 시 줌 인
		InterpFOV(DeltaTime);
	}
}



// Equip

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;

	// Drop Old Weapon
	if(EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	// EquippedWeapon 업데이트
	EquippedWeapon = WeaponToEquip;
	OnRep_EquippedWeapon(); // TODO Setter?
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon)
	{
		// New Weapon
		if(const auto HandSocket = Character->GetMesh()->GetSocketByName(FName("weapon_r")))
		{
			EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
			EquippedWeapon->SetOwner(Character);
		
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		// Equipped
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
	else
	{
		// UnEquipped
		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		Character->bUseControllerRotationYaw = false;
	}
}

// Crosshairs

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr) return;
	
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if(Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

		// 크로스헤어 텍스처
		if(HUD)
		{
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			// 크로스 헤어 색상
			// HitTarget 대상에 따른 크로스헤어 색상 변경
			HUDPackage.CrosshairsColor = HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableWithCrosshairs>()
			? FLinearColor::Red
			: FLinearColor::White;

			// 크로스헤어 분산도 계산
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			// 걷기 속도에 따른 분산도
			CrosshairsVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			// 공중에 뜬 상태에서의 분산도
			CrosshairsInAirFactor = Character->GetCharacterMovement()->IsFalling()
			? FMath::FInterpTo(CrosshairsInAirFactor, 2.25f, DeltaTime, 2.25f)
			: FMath::FInterpTo(CrosshairsInAirFactor, 0.f, DeltaTime, 30.f);

			// 조준 상태에서의 분산도
			CrosshairsAimFactor = bIsAiming
			? FMath::FInterpTo(CrosshairsAimFactor, .58f, DeltaTime, 30.f) // TODO 줌 인 보간 속도와 싱크?
			: FMath::FInterpTo(CrosshairsAimFactor, 0.f, DeltaTime, 30.f);

			// 총기 발사 시 분산도
			CrosshairsShootingFactor = FMath::FInterpTo(CrosshairsShootingFactor, 0.f, DeltaTime, 20.f);

			// 총 분산도
			HUDPackage.CrosshairsSpread =
				0.58f
				+ CrosshairsVelocityFactor
				+ CrosshairsInAirFactor
				- CrosshairsAimFactor
				+ CrosshairsShootingFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
		);

	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if(Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 25.f);
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility,
			CollisionQueryParams
			);

		// 허공을 바라보고 있는 경우
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

// Fire

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bIsFireButtonPressed = bPressed;

	if(bIsFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if(!bCanFire || EquippedWeapon == nullptr) return;
	bCanFire = false;

	CrosshairsShootingFactor = .75f;

	EquippedWeapon->RequestFire(HitTarget);
	ServerFire();
	StartFireTimer();
}

void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	
	Character->PlayFireMontage(bIsAiming);
	EquippedWeapon->Fire();
}

void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&ThisClass::FireTimerFinished,
		EquippedWeapon->GetFireDelay()
	);
}

void UCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	if(bIsFireButtonPressed && EquippedWeapon && EquippedWeapon->IsAutomatic())
	{
		Fire();
	}
}

// Aim

void UCombatComponent::IsAimingUpdated(bool IsAiming)
{
	bIsAiming = IsAiming;
	
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::SetIsAiming(bool IsAiming)
{
	ServerSetIsAiming(IsAiming);
	
	// RPC 호출 전에 해당 클라이언트에서는 미리 동작하도록 중복 호출
	IsAimingUpdated(IsAiming);
}

void UCombatComponent::ServerSetIsAiming_Implementation(bool IsAiming)
{
	IsAimingUpdated(IsAiming);
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(Character == nullptr || Character->GetFollowCamera() == nullptr || EquippedWeapon == nullptr) return;

	CurrentFOV = bIsAiming
	? FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed())
	: FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, DefaultZoomInterpSpeed);

	Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
}