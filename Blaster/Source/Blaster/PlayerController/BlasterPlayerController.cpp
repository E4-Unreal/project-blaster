// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/Character/CharacterOverlay.h"
#include "Blaster/HUD/Match/MatchTimerOverlay.h"
#include "Blaster/HUD/Character/WeaponOverlay.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsLocalController())
	{
		if(UWorld* World = GetWorld())
		{
			if(World->GetGameState())
			{
				SetBlasterGameState(World->GetGameState());
			}
			else
			{
				World->GameStateSetEvent.AddUObject(this, &ThisClass::SetBlasterGameState);
			}
		}
	}
}

void ABlasterPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	if(IsLocalController())
	{
		SetBlasterPlayerState(GetPlayerState<APlayerState>());
	}
}

void ABlasterPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if(IsLocalController())
	{
		SetBlasterPlayerState(GetPlayerState<APlayerState>());
	}
}

void ABlasterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	if(IsLocalController())
	{
		SetBlasterHUD(GetHUD());
	}
}

void ABlasterPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if(IsLocalController())
	{
		SetBlasterCharacter(P);
	}
}

void ABlasterPlayerController::SetBlasterGameState(AGameStateBase* GameState)
{
	// Blaster Game State 할당
	BlasterGameState = Cast<ABlasterGameState>(GameState);

	// Blaster HUD 할당 이후 초기화 진행
	if(BlasterHUD)
	{
		InitBlasterGameState();
	}
	else
	{
		OnBlasterHUDSet.AddUObject(this, &ThisClass::InitBlasterGameState);
	}
	
	OnBlasterGameStateSet.Broadcast();
}

void ABlasterPlayerController::InitBlasterGameState()
{
	if(BlasterGameState == nullptr) return;

	if(BlasterHUD)
	{
		BlasterGameState->OnCountdownUpdated.AddUObject(BlasterHUD, &ABlasterHUD::SetCountdownTime);
		BlasterGameState->OnTopScoringPlayersUpdated.AddUObject(BlasterHUD, &ABlasterHUD::SetTopScoringPlayers);
	}
}

void ABlasterPlayerController::SetBlasterCharacter(APawn* InPawn)
{
	// Blaster Character 할당
	BlasterCharacter = Cast<ABlasterCharacter>(InPawn);

	// Blaster HUD 할당 이후 초기화 진행
	if(BlasterHUD)
	{
		InitBlasterCharacter();
	}
	else
	{
		OnBlasterHUDSet.AddUObject(this, &ThisClass::InitBlasterCharacter);
	}
	
	OnBlasterCharacterSet.Broadcast();
}

void ABlasterPlayerController::InitBlasterCharacter()
{
	if(BlasterCharacter == nullptr || BlasterHUD == nullptr) return;
	
	BlasterCharacter->OnHealthUpdated.AddDynamic(BlasterHUD, &ABlasterHUD::SetHealth);
	BlasterCharacter->OnMaxHealthUpdated.AddDynamic(BlasterHUD, &ABlasterHUD::SetMaxHealth);
	BlasterCharacter->ManualUpdateHUD();
}

void ABlasterPlayerController::SetBlasterPlayerState(APlayerState* InPlayerState)
{
	// Blaster Player State 할당
	BlasterPlayerState = Cast<ABlasterPlayerState>(InPlayerState);
	if(BlasterPlayerState == nullptr) return;

	// Blaster HUD 할당 이후 초기화 진행
	if(BlasterHUD)
	{
		InitBlasterPlayerState();
	}
	else
	{
		OnBlasterHUDSet.AddUObject(this, &ThisClass::InitBlasterPlayerState);
	}

	OnBlasterPlayerStateSet.Broadcast();
}

void ABlasterPlayerController::InitBlasterPlayerState()
{
	if(BlasterPlayerState == nullptr) return;

	if(BlasterHUD)
	{
		BlasterPlayerState->OnScoreUpdated.AddDynamic(BlasterHUD, &ABlasterHUD::SetScore);
		BlasterPlayerState->OnDefeatsUpdated.AddDynamic(BlasterHUD, &ABlasterHUD::SetDefeats);
		BlasterPlayerState->ManualUpdateHUD(); // TODO 인터페이스화?
	}
}

void ABlasterPlayerController::SetBlasterHUD(AHUD* HUD)
{
	// Blaster HUD 할당
	BlasterHUD = Cast<ABlasterHUD>(HUD);
	if(BlasterHUD == nullptr) return;

	// Game State 할당 이후 초기화 진행
	if(BlasterGameState)
	{
		InitBlasterHUD();
	}
	else
	{
		OnBlasterGameStateSet.AddUObject(this, &ThisClass::InitBlasterHUD);
	}

	OnBlasterHUDSet.Broadcast();
}

void ABlasterPlayerController::InitBlasterHUD()
{
	if(BlasterGameState == nullptr) return;

	if(BlasterHUD)
	{
		BlasterGameState->OnMatchStateSet.AddDynamic(BlasterHUD, &ABlasterHUD::OnMatchStateSet);
		BlasterHUD->OnMatchStateSet(BlasterGameState->GetMatchState());
	}
}

/* Weapon Overlay */

void ABlasterPlayerController::SetHUDAmmo(int32 Ammo)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWeaponOverlay() == nullptr) return;

	BlasterHUD->GetWeaponOverlay()->Ammo = Ammo;
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWeaponOverlay() == nullptr) return;

	BlasterHUD->GetWeaponOverlay()->CarriedAmmo = CarriedAmmo;
}

void ABlasterPlayerController::SetHUDMagCapacity(int32 MagCapacity)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWeaponOverlay() == nullptr) return;

	BlasterHUD->GetWeaponOverlay()->MagCapacity = MagCapacity;
}

void ABlasterPlayerController::HideWeaponOverlay() const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWeaponOverlay() == nullptr) return;

	BlasterHUD->GetWeaponOverlay()->SetVisibility(ESlateVisibility::Hidden);
}

void ABlasterPlayerController::ShowWeaponOverlay() const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWeaponOverlay() == nullptr) return;

	BlasterHUD->GetWeaponOverlay()->SetVisibility(ESlateVisibility::Visible);
}
