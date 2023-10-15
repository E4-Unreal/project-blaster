// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponOverlay.h"

#include "Blaster/Weapon/Weapon.h"

void UWeaponOverlay::SetEquippedWeapon(AWeapon* InEquippedWeapon)
{
	if(EquippedWeapon)
	{
		if(EquippedWeapon->OnAmmoUpdated.IsAlreadyBound(this, &ThisClass::SetAmmo))
			EquippedWeapon->OnAmmoUpdated.RemoveDynamic(this, &ThisClass::SetAmmo);
	}
	
	EquippedWeapon = InEquippedWeapon;
	
	if(EquippedWeapon == nullptr)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
		WeaponName = EquippedWeapon->GetWeaponName();
		EquippedWeapon->OnAmmoUpdated.AddDynamic(this, &ThisClass::SetAmmo);
		EquippedWeapon->ManualUpdateHUD();
	}
}

void UWeaponOverlay::SetGrenadeCount(int32 InGrenadeCount)
{
	GrenadeCount = InGrenadeCount;
}

void UWeaponOverlay::SetCarriedAmmo(int32 InCarriedAmmo)
{
	CarriedAmmo = InCarriedAmmo;
}

void UWeaponOverlay::SetAmmo(int32 InAmmo)
{
	Ammo = InAmmo;
}
