// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreUpdatedSiganature, int32, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDefeatsUpdatedSiganature, int32, Defeats);

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void AddScore(float ScoreAmount = 1.0f);
	void AddDefeats(int32 DefeatsAmount = 1);

	virtual void OnRep_Score() override;
	
	UFUNCTION()
	virtual void OnRep_Defeats();

	/* For UI */
	UPROPERTY(BlueprintAssignable, Category = UI)
	FScoreUpdatedSiganature OnScoreUpdated;

	UPROPERTY(BlueprintAssignable, Category = UI)
	FDefeatsUpdatedSiganature OnDefeatsUpdated;

	void ManualUpdateHUD();

private:
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
