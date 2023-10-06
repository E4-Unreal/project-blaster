// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void ClientInitialize(AController* C) override;

	void UpdateHUDScore();
	void AddToScore(float ScoreAmount);
	virtual void OnRep_Score() override;

protected:
	virtual void BeginPlay() override;

private:
	class ABlasterPlayerController* Controller;
};
