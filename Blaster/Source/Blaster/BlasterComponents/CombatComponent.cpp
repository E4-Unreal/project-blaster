// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
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
}

