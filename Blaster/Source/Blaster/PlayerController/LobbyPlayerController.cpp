// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "Blaster/HUD/BlasterHUD.h"
#include "GameFramework/GameMode.h"


void ALobbyPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	if(GetBlasterHUD())
	{
		GetBlasterHUD()->OnMatchStateSet(MatchState::InProgress);
		GetBlasterHUD()->HideAllExceptCharacterInfo();
	}
}
