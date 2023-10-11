// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Defeats);
}

void ABlasterPlayerState::AddScore(float ScoreAmount)
{
	if(!HasAuthority()) return; // TODO 없애도 되지 않나? 점수 업데이트 딜레이 문제 해결될 것 같음
	
	SetScore(GetScore() + ScoreAmount);
	OnScoreUpdated.Broadcast(FMath::FloorToInt32(GetScore()));
}

void ABlasterPlayerState::AddDefeats(int32 DefeatsAmount)
{
	if(!HasAuthority()) return; // TODO 없애도 되지 않나? 점수 업데이트 딜레이 문제 해결될 것 같음

	Defeats += DefeatsAmount;
	OnDefeatsUpdated.Broadcast(Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnScoreUpdated.Broadcast(FMath::FloorToInt32(GetScore()));
}

void ABlasterPlayerState::OnRep_Defeats()
{
	OnDefeatsUpdated.Broadcast(Defeats);
}

void ABlasterPlayerState::ManualUpdateHUD()
{
	OnScoreUpdated.Broadcast(FMath::FloorToInt32(GetScore()));
	OnDefeatsUpdated.Broadcast(Defeats);
}