// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponOverlay.generated.h"

class AWeapon;

/**
 * 
 */
UCLASS()
class BLASTER_API UWeaponOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetEquippedWeapon(AWeapon* InEquippedWeapon);

	void SetGrenadeCount(int32 InGrenadeCount);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Ammo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CarriedAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MagCapacity;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 GrenadeCount;
};
