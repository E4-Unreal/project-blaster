// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMatchStateSetSignature, FName, NewState);

class ABlasterPlayerState;

UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABlasterGameState();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_MatchState() override;
	
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);

	UPROPERTY(BlueprintAssignable)
	FMatchStateSetSignature OnMatchStateSet;

	DECLARE_EVENT_OneParam(ThisClass, FCountdownUpdatedEvent, float);
	FCountdownUpdatedEvent OnCountdownUpdated;
	
	DECLARE_EVENT_OneParam(ThisClass, FTopScoringPlayersUpdatedEvent, const TArray<ABlasterPlayerState*>&);
	FTopScoringPlayersUpdatedEvent OnTopScoringPlayersUpdated;

private:
	/* Match 타이머 */
	UPROPERTY(Replicated)
	float WarmupTime = 0.f;

	UPROPERTY(Replicated)
	float MatchTime = 0.f;

	UPROPERTY(Replicated)
	float CooldownTime = 0.f;

	uint32 CountdownTimeInt = 0;

	/* Score */
	float TopScore = 0.f;

	// TODO 클라이언트에 Player State 있지 않나?
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;
};
