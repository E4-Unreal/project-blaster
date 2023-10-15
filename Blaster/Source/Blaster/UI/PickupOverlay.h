// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UPickupOverlay : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName WeaponName;

public:
	void SetWeaponName(FName InWeaponName);
};
