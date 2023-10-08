// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UWeaponOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Ammo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CarriedAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MagCapacity;
};
