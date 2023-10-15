// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"

#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

ABlasterGameState::ABlasterGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlasterGameState::Tick(float DeltaSeconds)
{
	UE_LOG(LogTemp, Error, TEXT("ABlasterGameState::Tick"))
	Super::Tick(DeltaSeconds);
	
	if(UWorld* World = GetWorld())
	{
		// TODO 리팩토링?
		// 매치 상태에 따른 남은 시간 계산
		float CountdownTime = 0.f;
		if(MatchState == MatchState::WaitingToStart)
		{
			const float Timer = GetServerWorldTimeSeconds() - WarmupStartTime;
			CountdownTime = FMath::Clamp(WarmupTime - Timer, 0.f, WarmupTime);
		}
		else if(MatchState == MatchState::InProgress)
		{
			const float Timer = GetServerWorldTimeSeconds() - MatchStartTime;
			CountdownTime = FMath::Clamp(MatchTime - Timer, 0.f, MatchTime);
		}
		else if(MatchState == MatchState::Cooldown)
		{
			const float Timer = GetServerWorldTimeSeconds() - CooldownStartTime;
			CountdownTime = FMath::Clamp( CooldownTime - Timer, 0.f, CooldownTime);
		}
		
		const uint32 SecondsLeftInt = FMath::FloorToInt(CountdownTime);

		// Countdown Time을 1초 단위로 업데이트
		if(CountdownTimeInt != SecondsLeftInt)
		{
			CountdownTimeInt = SecondsLeftInt;
			
			OnCountdownUpdated.Broadcast(CountdownTimeInt);
		}
	}

}

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WarmupTime);
	DOREPLIFETIME(ThisClass, MatchTime);
	DOREPLIFETIME(ThisClass, CooldownTime);
	DOREPLIFETIME(ThisClass, TopScoringPlayers);
	DOREPLIFETIME(ThisClass, MatchStartTime);
	DOREPLIFETIME(ThisClass, CooldownStartTime);
	DOREPLIFETIME(ThisClass, WarmupStartTime);
}

void ABlasterGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	
	OnMatchStateSet.Broadcast(MatchState);
	
	if(MatchState == MatchState::WaitingToStart)
	{
		if(UWorld* World = GetWorld())
		{
			// 서버에서만 실행
			if(World->GetAuthGameMode())
			{
				if(const ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(World->GetAuthGameMode()))
				{
					WarmupTime = GameMode->WarmupTime;
					MatchTime = GameMode->MatchTime;
					CooldownTime = GameMode->CooldownTime;
				}
			}
		}
	}
	else if(MatchState == MatchState::Cooldown) // TODO WaitingPostMatch
	{
		OnTopScoringPlayersUpdated.Broadcast(TopScoringPlayers);
	}
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if(TopScoringPlayers.IsEmpty())
	{
		TopScoringPlayers.Emplace(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if(ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if(ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.Emplace(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}