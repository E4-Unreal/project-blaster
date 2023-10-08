// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsText;

	UPROPERTY(meta = (BindWidget))
	class UWeaponOverlay* WeaponOverlay;

	void SetAmmo(int32 Ammo);
	void SetCarriedAmmo(int32 CarriedAmmo);
	void SetMagCapacity(int32 MagCapacity);
	void ShowWeaponOverlay();
	void HideWeaponOverlay();
};
