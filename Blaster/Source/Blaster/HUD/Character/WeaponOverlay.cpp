// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponOverlay.h"

void UWeaponOverlay::SetEquippedWeapon(AWeapon* InEquippedWeapon)
{
	EquippedWeapon = InEquippedWeapon;
	if(EquippedWeapon == nullptr)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}
