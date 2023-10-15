// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterPlayerController.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "LobbyPlayerController.generated.h"

UCLASS()
class BLASTER_API ALobbyPlayerController : public ABlasterPlayerController
{
	GENERATED_BODY()

public:
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
};
