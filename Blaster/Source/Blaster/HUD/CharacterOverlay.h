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
	// 사용자 위젯
	UPROPERTY(meta = (BindWidget))
	class UWeaponOverlay* WeaponOverlay;

	UPROPERTY(meta = (BindWidget))
	class UMatchTimerOverlay* MatchTimerOverlay;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsText;

	// Weapon Overlay
	void SetAmmo(int32 Ammo);
	void SetCarriedAmmo(int32 CarriedAmmo);
	void SetMagCapacity(int32 MagCapacity);
	void ShowWeaponOverlay();
	void HideWeaponOverlay();

	// Match Timer Overlay
	void SetCountdownTime(float CountdownTime);
};
