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
	/* 멤버 변수 할당 이벤트 */
	virtual void PostInitializeComponents() override; // Set Game State
	virtual void InitPlayerState() override; // Set Player State
	virtual void OnRep_PlayerState() override; // Set Player State
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override; // Set HUD
	virtual void AcknowledgePossession(APawn* P) override; // Set Pawn

private:
	/* UI Model */
	// Blaster Game State
	class ABlasterGameState* BlasterGameState;
	void SetBlasterGameState(AGameStateBase* GameState);
	void InitBlasterGameState();
	
	DECLARE_EVENT(ThisClass, FGameStateSetEvent);
	FGameStateSetEvent OnBlasterGameStateSet;

	// Blaster Character
	class ABlasterCharacter* BlasterCharacter;
	void SetBlasterCharacter(APawn* InPawn);
	void InitBlasterCharacter();

	DECLARE_EVENT(ThisClass, FCharacterSetEvent);
	FCharacterSetEvent OnBlasterCharacterSet;

	// Blaster Player State
	class ABlasterPlayerState* BlasterPlayerState;
	void SetBlasterPlayerState(APlayerState* InPlayerState);
	void InitBlasterPlayerState();

	DECLARE_EVENT(ThisClass, FPlayerStateSetEvent);
	FPlayerStateSetEvent OnBlasterPlayerStateSet;

	/* UI View */
	// HUD
	class ABlasterHUD* BlasterHUD;
	void SetBlasterHUD(AHUD* HUD);
	void InitBlasterHUD();
	
	DECLARE_EVENT(ThisClass, FHUDSetEvent);
	FHUDSetEvent OnBlasterHUDSet;

public:
	// TODO BlasterHUD로 옮길 예정
	// Weapon
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDMagCapacity(int32 MagCapacity);
	void HideWeaponOverlay() const;
	void ShowWeaponOverlay() const;
};
