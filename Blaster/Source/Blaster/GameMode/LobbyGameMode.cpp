// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if(NumberOfPlayers == PlayersForStart)
	{
		ServerTravelToBlasterMap();
	}
}

APawn* ALobbyGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* DefaultPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(DefaultPawn);
	if(BlasterCharacter && BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->GetCombatComponent()->ApplyStartingAmmo(StartingAmmo);
	}

	return DefaultPawn;
}

void ALobbyGameMode::ServerTravelToBlasterMap()
{
	if(UWorld* World = GetWorld())
	{
		bUseSeamlessTravel = true;
		World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
	}
}
