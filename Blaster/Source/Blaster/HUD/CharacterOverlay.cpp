// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"

#include "WeaponOverlay.h"

void UCharacterOverlay::SetAmmo(int32 Ammo)
{
	if(WeaponOverlay)
		WeaponOverlay->Ammo = Ammo;
}

void UCharacterOverlay::SetCarriedAmmo(int32 CarriedAmmo)
{
	if(WeaponOverlay)
		WeaponOverlay->CarriedAmmo = CarriedAmmo;
}

void UCharacterOverlay::SetMagCapacity(int32 MagCapacity)
{
	if(WeaponOverlay)
		WeaponOverlay->MagCapacity = MagCapacity;
}

void UCharacterOverlay::ShowWeaponOverlay()
{
	if(WeaponOverlay)
		WeaponOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UCharacterOverlay::HideWeaponOverlay()
{
	if(WeaponOverlay)
		WeaponOverlay->SetVisibility(ESlateVisibility::Hidden);
}
