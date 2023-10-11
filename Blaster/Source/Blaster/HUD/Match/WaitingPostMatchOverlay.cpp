// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitingPostMatchOverlay.h"

#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UWaitingPostMatchOverlay::SetTopScoringPlayers(const TArray<ABlasterPlayerState*>& InTopScoringPlayers)
{
	TopScoringPlayers = InTopScoringPlayers;
	APlayerState* LocalPlayerState = UGameplayStatics::GetPlayerController(this, 0)->PlayerState;
	
	if(TopScoringPlayers.Num() == 0)
	{
		WinnerState = EWinnerState::EWS_NoOne;
	}
	else if(TopScoringPlayers.Contains(Cast<ABlasterPlayerState>(LocalPlayerState)))
	{
		WinnerState = EWinnerState::EWS_LocalPlayer;
	}
	else
	{
		WinnerState = EWinnerState::EWS_RemotePlayer;
	}
}
