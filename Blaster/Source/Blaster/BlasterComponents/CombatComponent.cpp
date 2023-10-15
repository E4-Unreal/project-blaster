// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/Projectile/Projectile.h"
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
	DOREPLIFETIME(ThisClass, CarriedAmmo);
	DOREPLIFETIME(ThisClass, CombatState);
	DOREPLIFETIME(ThisClass, Grenades);
}

void UCombatComponent::ApplyStartingAmmo(FStartingAmmo InStartingAmmo)
{
	// TODO 리팩토링?
	StartingAmmo = InStartingAmmo;
	CarriedAmmoMap.Emplace(EWeaponType::EW_AssaultRifle, InStartingAmmo.StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_RocketLauncher, InStartingAmmo.StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_Pistol, InStartingAmmo.StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_SMG, InStartingAmmo.StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_ShotGun, InStartingAmmo.StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_SniperRifle, InStartingAmmo.StartingSniperRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_GrenadeLauncher, InStartingAmmo.StartingGrenadeLauncherAmmo);
	Grenades = StartingAmmo.Grenades;

	if(EquippedWeapon)
	{
		SetCarriedAmmo(
			CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType())
			? CarriedAmmoMap[EquippedWeapon->GetWeaponType()]
			: 0
		);
	}

	bAppliedStartingAmmo = true;
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

		if(Character->HasAuthority())
		{
			InitializeCarriedAmmo();
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

/* Equip */

// 서버에서만 호출할 것
void UCombatComponent::ServerEquipWeapon_Implementation(AWeapon* NewWeapon)
{
	if(Character == nullptr || NewWeapon == nullptr || CombatState != ECombatState::ECS_Unoccupied) return;

	DropWeapon(EquippedWeapon);
	EquipWeapon(NewWeapon);
	SetEquippedWeapon(NewWeapon);
	Character->Equipped();
}

void UCombatComponent::SetEquippedWeapon(AWeapon* NewWeapon)
{
	AWeapon* OldWeapon = EquippedWeapon;
	EquippedWeapon = NewWeapon;
	OnRep_EquippedWeapon(OldWeapon);
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* OldWeapon)
{
	if(Character == nullptr) return;

	if(EquippedWeapon)
	{
		AttachWeaponToSocket(EquippedWeapon, EWeaponSocket::EWS_RightHand);
		Character->Equipped();
		EquippedWeapon->PlayEquipSound();
	}
	else
	{
		Character->UnEquipped();
	}

	OnEquippedWeaponUpdated.Broadcast(EquippedWeapon);
}

void UCombatComponent::EquipWeapon(AWeapon* NewWeapon)
{
	if(NewWeapon == nullptr || Character == nullptr || !Character->HasAuthority()) return;
	
	NewWeapon->SetOwner(Character); 
	NewWeapon->SetInstigator(Character);
	
	SetCarriedAmmo(
		CarriedAmmoMap.Contains(NewWeapon->GetWeaponType())
		? CarriedAmmoMap[NewWeapon->GetWeaponType()]
		: 0
	);
}

void UCombatComponent::ServerDropWeapon_Implementation()
{
	if(Character == nullptr) return;
	
	DropWeapon(EquippedWeapon);
	SetEquippedWeapon(nullptr);
	Character->UnEquipped();
}

void UCombatComponent::DropWeapon(AWeapon* OldWeapon)
{
	if(OldWeapon == nullptr || Character == nullptr || !Character->HasAuthority()) return;

	OldWeapon->SetOwner(nullptr);
	OldWeapon->SetInstigator(nullptr);

	SetCarriedAmmo(0);
}

void UCombatComponent::AttachWeaponToSocket(AWeapon* Weapon, EWeaponSocket Socket)
{
	if(Weapon == nullptr || Character == nullptr || Character->GetMesh() == nullptr) return;
	
	USkeletalMeshSocket const* TargetSocket;

	switch (Socket)
	{
	case EWeaponSocket::EWS_RightHand:
		TargetSocket = Character->GetRightHandSocket();
		break;
	case EWeaponSocket::EWS_LeftHand:
		TargetSocket = Character->GetLeftHandSocket();
		break;
	default:
		TargetSocket = nullptr;
		break;
	}

	if(TargetSocket == nullptr) return;
	Weapon->EnableCollisionAndPhysics(false);
	TargetSocket->AttachActor(Weapon, Character->GetMesh());
}

void UCombatComponent::ManualUpdateHUD()
{
	OnEquippedWeaponUpdated.Broadcast(EquippedWeapon);
	OnGrenadesUpdated.Broadcast(Grenades);
	OnCarriedAmmoUpdated.Broadcast(CarriedAmmo);
	
	if(EquippedWeapon)
	{
		EquippedWeapon->ManualUpdateHUD();
	}
}

void UCombatComponent::Reload()
{
	if(!CanReload()) return;
	
	ServerReload();
}

void UCombatComponent::AutoReload()
{
	if(EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if(!CanReload()) return;

	// TODO 굳이 서버에서만 할 필요가 있나? 멀티 캐스트로 한 뒤 Ammo만 Replicate 하면 되지 않나?
	CombatState = ECombatState::ECS_Reloading;
	MulticastReload();
}

void UCombatComponent::MulticastReload_Implementation()
{
	Character->PlayReloadMontage();
}

// TODO 멀티 캐스트?
void UCombatComponent::FinishReloading()
{
	if(Character == nullptr) return;

	if(Character->HasAuthority())
	{
		// Reloading 상태 해제
		CombatState = ECombatState::ECS_Unoccupied;

		// 서버 플레이어의 Fire 입력 여부 확인
		if(bIsFireButtonPressed && Character->IsLocallyControlled())
		{
			Fire();
		}
	}
}

void UCombatComponent::AddAmmoToWeapon()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;

	// TODO TMap 대신 서버 클라이언트 모두 모든 종류의 총알을 보관하는 방법은 없을까
	// Server
	if(Character->HasAuthority())
	{
		// Map 변경 후 Carried Ammo 업데이트
		const int32 Amount = EquippedWeapon->GetWeaponType() == EWeaponType::EW_ShotGun ? 1 : GetAmountToReload();
		if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()) && CarriedAmmoMap[EquippedWeapon->GetWeaponType()] >= Amount)
		{
			CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= Amount;
			SetCarriedAmmo(CarriedAmmoMap[EquippedWeapon->GetWeaponType()]);
			EquippedWeapon->AddAmmo(Amount);
		}
	}
	
	// 샷건만 적용
	if(EquippedWeapon->GetWeaponType() == EWeaponType::EW_ShotGun)
	{
		if(EquippedWeapon->IsFull() || CarriedAmmo == 0)
		{
			Character->Montage_JumpToSection(FName("ShotgunEnd"));
		}
	}
}

void UCombatComponent::OnRep_CombatState(ECombatState OldState)
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		break;
	case ECombatState::ECS_Unoccupied:
		if(OldState == ECombatState::ECS_Reloading)
		{
			// 클라이언트 플레이어의 Fire 입력 여부 확인
			if(bIsFireButtonPressed)
			{
				Fire();
			}
		}
		break;
	}
}

void UCombatComponent::SetGrenades(int32 InGrenades)
{
	Grenades = InGrenades;

	OnRep_Grenades();
}

void UCombatComponent::OnRep_Grenades()
{
	OnGrenadesUpdated.Broadcast(Grenades);
}

bool UCombatComponent::CanReload() const
{
	if(EquippedWeapon == nullptr) return false;
	
	bool bCombatCanReload = CombatState == ECombatState::ECS_Unoccupied && CarriedAmmo > 0;
	
	return bCombatCanReload && EquippedWeapon->CanReload();
}

void UCombatComponent::ThrowGrenade()
{
	if(CombatState != ECombatState::ECS_Unoccupied || Grenades <= 0 || EquippedWeapon == nullptr) return;

	// For Local Client
	if(Character && !Character->HasAuthority())
	{
		SetGrenades(Grenades - 1);
	}
	
	ServerThrowGrenade();
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	CombatState = ECombatState::ECS_ThrowingGrenade;
	SetGrenades(Grenades - 1);
	MulticastThrowGrenade();
}

void UCombatComponent::MulticastThrowGrenade_Implementation()
{
	if(Character)
	{
		Character->PlayThrowGrenadeMontage();
		SpawnGrenade();
		AttachWeaponToSocket(EquippedWeapon, EWeaponSocket::EWS_LeftHand);
	}
}

void UCombatComponent::SpawnGrenade()
{
	if(Character == nullptr || Character->GetAttachedGrenade() == nullptr) return;

	const AProjectile* GrenadeCDO = Cast<AProjectile>(GrenadeClass->GetDefaultObject(true));
	if(GrenadeCDO->GetProjectileMesh())
	{
		Character->GetAttachedGrenade()->SetStaticMesh(
		GrenadeCDO->GetProjectileMesh()->GetStaticMesh()
		);
		Character->GetAttachedGrenade()->SetVisibility(true);
	}
}

void UCombatComponent::LaunchGrenade()
{
	if(Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetStaticMesh(nullptr);
		Character->GetAttachedGrenade()->SetVisibility(false);

		if(Character->IsLocallyControlled())	
			ServerLaunchGrenade(Character->GetAttachedGrenade()->GetComponentLocation(), HitTarget);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& SpawnLocation, const FVector_NetQuantize& Target)
{
	if(GrenadeClass == nullptr) return;
	
	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;

		FVector Direction = Target - SpawnLocation;
		
		World->SpawnActor<AProjectile>(
			GrenadeClass,
			SpawnLocation,
			Direction.Rotation(),
			SpawnParams
		);
	}
}

void UCombatComponent::FinishThrowingGrenade()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachWeaponToSocket(EquippedWeapon, EWeaponSocket::EWS_RightHand);
}

int32 UCombatComponent::GetAmountToReload()
{
	if(EquippedWeapon == nullptr) return 0;
	
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	return FMath::Clamp(FMath::Min(RoomInMag, CarriedAmmo), 0, CarriedAmmo);
}

/* Crosshairs */

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

		if(UWorld* World = GetWorld())
		{
			World->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility,
			CollisionQueryParams
			);
		}

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
	if(!CanFire() || EquippedWeapon == nullptr) return;

	// 무기 상태 확인
	if(!EquippedWeapon->CanFire())
	{
		if(EquippedWeapon->IsEmpty())
		{
			Reload();
		}

		return;
	}

	// Server RPC
	EquippedWeapon->Fire(HitTarget);
	ServerFire();

	// For Local Player
	CrosshairsShootingFactor = .75f;
	StartFireTimer();
}

void UCombatComponent::ServerFire_Implementation()
{
	if(EquippedWeapon == nullptr) return;
	
	// TODO 리팩토링?
	// For Shotgun
	if(EquippedWeapon->GetWeaponType() == EWeaponType::EW_ShotGun)
	{
		bCanFire = true;
		CombatState = ECombatState::ECS_Unoccupied;
	}
	
	MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;

	// Handle Fire (Character)
	Character->PlayFireMontage(bIsAiming);
}

bool UCombatComponent::CanFire()
{
	if(EquippedWeapon == nullptr) return false;

	// TODO 리팩토링?
	return EquippedWeapon->GetWeaponType() == EWeaponType::EW_ShotGun
	? bCanFire // Reload 상태에서도 샷건은 발사 가능
	: bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;

	bCanFire = false;
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
	AutoReload();
}

void UCombatComponent::SetCarriedAmmo(int32 InCarriedAmmo)
{
	CarriedAmmo = InCarriedAmmo;
	OnCarriedAmmoUpdated.Broadcast(CarriedAmmo);
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	OnCarriedAmmoUpdated.Broadcast(CarriedAmmo);
}

void UCombatComponent::InitializeCarriedAmmo()
{
	if(bAppliedStartingAmmo) return;
	
	// TODO TMap For Loop로 설정?
	CarriedAmmoMap.Emplace(EWeaponType::EW_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_ShotGun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_SniperRifle, StartingSniperRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EW_GrenadeLauncher, StartingGrenadeLauncherAmmo);

	if(EquippedWeapon)
	{
		SetCarriedAmmo(
			CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType())
			? CarriedAmmoMap[EquippedWeapon->GetWeaponType()]
			: 0
		);
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

	// Local Player
	if(Character && Character->IsLocallyControlled() && EquippedWeapon && EquippedWeapon->GetWeaponType() == EWeaponType::EW_SniperRifle)
	{
		Character->ShowSniperScopeWidget(IsAiming);
	}
	
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