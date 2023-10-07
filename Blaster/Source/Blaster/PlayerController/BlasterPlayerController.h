// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	virtual void BeginPlayingState() override;

	// Character
	void SetHUDHealth(float Health, float MaxHealth);

	// Weapon
	void SetHUDAmmo(int32 Ammo);
	void SetHUDMagCapacity(int32 MagCapacity);
	void ClearHUDWeapon();

	// Player State
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

private:
	class ABlasterCharacter* BlasterCharacter;
	class ABlasterHUD* BlasterHUD;
	class ABlasterPlayerState* BlasterPlayerState;
};
