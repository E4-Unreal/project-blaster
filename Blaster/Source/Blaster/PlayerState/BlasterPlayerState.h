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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ClientInitialize(AController* C) override;

	void AddToScore(float ScoreAmount);
	void UpdateHUDScore();

	void AddToDefeats(int32 DefeatsAmount);
	void UpdateHUDDefeats();

	// 레플리케이션 노티파이
	virtual void OnRep_Score() override;
	
	UFUNCTION()
	virtual void OnRep_Defeats();

protected:
	virtual void BeginPlay() override;

private:
	class ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	void Initialize();
};
