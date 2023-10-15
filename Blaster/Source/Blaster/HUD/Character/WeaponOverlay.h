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
	UFUNCTION()
	void SetEquippedWeapon(AWeapon* InEquippedWeapon);

	UFUNCTION()
	void SetGrenadeCount(int32 InGrenadeCount);

	UFUNCTION()
	void SetCarriedAmmo(int32 InCarriedAmmo);

	UFUNCTION()
	void SetAmmo(int32 InAmmo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MagCapacity;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 GrenadeCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 CarriedAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName WeaponName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Ammo;
};
