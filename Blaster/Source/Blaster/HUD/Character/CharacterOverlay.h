// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UTextBlock;
class UWeaponOverlay;
class UMatchTimerOverlay;

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	// TODO Player State Overlay
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsText;

	// TODO 임시 for Lobby
	void HideAllExceptCharacterInfo() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Health;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float HealthRatio;
		
	// 자식 사용자 위젯
	UPROPERTY(meta = (BindWidget))
	UWeaponOverlay* WeaponOverlay;

	UPROPERTY(meta = (BindWidget))
	UMatchTimerOverlay* MatchTimerOverlay;

public:
	FORCEINLINE UWeaponOverlay* GetWeaponOverlay() const { return WeaponOverlay; }
	FORCEINLINE UMatchTimerOverlay* GetMatchTimerOverlay() const { return MatchTimerOverlay; }

	void SetHealth(const float InHealth);
	void SetMaxHealth(const float InMaxHealth);
};
