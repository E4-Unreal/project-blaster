// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
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

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, bIsAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;

	// Old Weapon 설정
	if(EquippedWeapon)
	{
		// TODO Drop Old Weapon
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
		EquippedWeapon->SetOwner(nullptr);
	}
	
	const auto HandSocket = Character->GetMesh()->GetSocketByName(FName("weapon_r"));
	if(HandSocket)
	{
		// New Weapon 설정
		WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);
		WeaponToEquip->SetOwner(Character);
		
		HandSocket->AttachActor(WeaponToEquip, Character->GetMesh());
	}

	// EquippedWeapon 업데이트
	EquippedWeapon = WeaponToEquip;
	OnRep_EquippedWeapon(); // TODO Setter?
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
	else
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		Character->bUseControllerRotationYaw = false;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bIsFireButtonPressed = bPressed;

	if(bIsFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
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
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
			);
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon == nullptr) return;
	
	if(Character)
	{
		Character->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::UpdateIsAiming(bool IsAiming)
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
	UpdateIsAiming(IsAiming);
}

void UCombatComponent::ServerSetIsAiming_Implementation(bool IsAiming)
{
	UpdateIsAiming(IsAiming);
}
