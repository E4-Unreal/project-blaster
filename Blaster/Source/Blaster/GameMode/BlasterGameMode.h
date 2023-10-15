// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match 시간이 종료되어 Match가 끝난 상태. Match 결과를 표시한다.
}

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void InitGameState() override;
	
	virtual void PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	virtual void OnMatchStateSet() override;

	/* Match 정보 */
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

private:
	class ABlasterGameState* BlasterGameState;
	
	float CountdownTime = 0.f;

	float WarmupStartTime;
	float CooldownStartTime;
	float MatchStartTime;

	UPROPERTY(EditAnywhere)
	int32 MatchScore = 5;

	UPROPERTY(EditAnywhere)
	int32 MaxMatchScore = 10;
};
