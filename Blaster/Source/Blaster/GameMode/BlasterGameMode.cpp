// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	UE_LOG(LogTemp, Error, TEXT("ABlasterGameMode::Tick"))
	Super::Tick(DeltaSeconds);

	if(UWorld* World = GetWorld())
	{
		// TODO 리팩토링?
		if(MatchState == MatchState::WaitingToStart)
		{
			CountdownTime = WarmupTime - (World->GetTimeSeconds() - WarmupStartTime);
			if(CountdownTime <= 0.f)
			{
				StartMatch();
			}
		}
		else if(MatchState == MatchState::InProgress)
		{
			CountdownTime = MatchTime - (World->GetTimeSeconds() - MatchStartTime);
			if(CountdownTime <= 0.f)
			{
				SetMatchState(MatchState::Cooldown);
			}
		}
		else if(MatchState == MatchState::Cooldown)
		{
			CountdownTime = CooldownTime - (World->GetTimeSeconds() - CooldownStartTime);
			if(CountdownTime <= 0.f)
			{
				RestartGame();
			}
		}
	}

}

void ABlasterGameMode::InitGameState()
{
	Super::InitGameState();

	BlasterGameState = GetGameState<ABlasterGameState>();
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter,
                                        ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if(MatchState != MatchState::InProgress) return;
	
	// TODO Score
	ABlasterPlayerState* AttackerPlayerState = AttackerController
	? Cast<ABlasterPlayerState>(AttackerController->PlayerState)
	: nullptr;

	ABlasterPlayerState* VictimPlayerState = VictimController
	? Cast<ABlasterPlayerState>(VictimController->PlayerState)
	: nullptr;

	if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddScore();
		
		if(BlasterGameState)
		{
			BlasterGameState->UpdateTopScore(AttackerPlayerState);
		}
	}

	if(VictimPlayerState)
	{
		VictimPlayerState->AddDefeats();
	}

	if(EliminatedCharacter)
		EliminatedCharacter->ServerEliminate();

	if(BlasterGameState)
	{
		if(BlasterGameState->GetTopScore() >= FMath::Min(MatchScore * (GameState.Get()->PlayerArray.Num() - 1), MaxMatchScore))
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if(EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
	}

	if(EliminatedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	UE_LOG(LogTemp, Error, TEXT("ABlasterGameMode::OnMatchStateSet : %s"), *MatchState.ToString())
	Super::OnMatchStateSet();

	if(UWorld* World = GetWorld())
	{
		if(MatchState == MatchState::WaitingToStart)
		{
			WarmupStartTime = World->GetTimeSeconds();
			if(BlasterGameState)
			{
				BlasterGameState->WarmupStartTime = WarmupStartTime;
			}
		}
		else if(MatchState == MatchState::InProgress)
		{
			MatchStartTime = World->GetTimeSeconds();
			if(BlasterGameState)
			{
				BlasterGameState->MatchStartTime = MatchStartTime;
			}
		}
		else if(MatchState == MatchState::Cooldown)
		{
			CooldownStartTime = World->GetTimeSeconds();;
			if(BlasterGameState)
			{
				BlasterGameState->CooldownStartTime = CooldownStartTime;
			}
		}
	}
}
