// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"

void UCharacterOverlay::SetHealth(const float InHealth)
{
	Health = InHealth;
	HealthRatio = MaxHealth == 0.f ? 1.f : Health / MaxHealth;
}

void UCharacterOverlay::SetMaxHealth(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
	HealthRatio = MaxHealth == 0.f ? 1.f : Health / MaxHealth;
}
