// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UFUNCTION(BlueprintCallable)
	void ServerTravelToBlasterMap();

private:
	UPROPERTY(EditAnywhere)
	int32 PlayersForStart = 2;

	UPROPERTY(EditAnywhere)
	FStartingAmmo StartingAmmo;
};
