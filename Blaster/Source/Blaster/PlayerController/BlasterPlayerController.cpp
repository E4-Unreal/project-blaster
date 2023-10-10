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
	
	BlasterGameState->OnCountdownUpdated.AddUObject(this, &ThisClass::SetHUD_CountdownTime);
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
	if(BlasterCharacter == nullptr) return;
	
	BlasterCharacter->OnHealthUpdated.AddDynamic(this, &ThisClass::SetHUD_Health);
	BlasterCharacter->OnMaxHealthUpdated.AddDynamic(this, &ThisClass::SetHUD_MaxHealth);
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

	BlasterPlayerState->UpdateHUD_All();
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
	if(BlasterGameState == nullptr || BlasterHUD == nullptr) return;

	BlasterGameState->OnMatchStateSet.AddDynamic(BlasterHUD, &ABlasterHUD::OnMatchStateSet);
	BlasterHUD->OnMatchStateSet(BlasterGameState->GetMatchState());
}

/* HUD */

void ABlasterPlayerController::UpdateHUD_All()
{

	if(BlasterPlayerState)
	{
		BlasterPlayerState->UpdateHUD_All();
	}

	// Hide Overlays
	HideWeaponOverlay();
}

/* Character Overlay */

void ABlasterPlayerController::SetHUD_Health(float Health)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetCharacterOverlay() == nullptr) return;
	
	BlasterHUD->GetCharacterOverlay()->SetHealth(Health);
}

void ABlasterPlayerController::SetHUD_MaxHealth(float MaxHealth)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetCharacterOverlay() == nullptr) return;
	
	BlasterHUD->GetCharacterOverlay()->SetMaxHealth(MaxHealth);
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

/* Player State Overlay */

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD
	&& BlasterHUD->GetCharacterOverlay()
	&& BlasterHUD->GetCharacterOverlay()->ScoreText;

	if(bHUDValid)
	{
		const FString ScoreString = FString::Printf(TEXT("Score : %d"), FMath::FloorToInt(Score));
		BlasterHUD->GetCharacterOverlay()->ScoreText->SetText(FText::FromString(ScoreString));
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD
	&& BlasterHUD->GetCharacterOverlay()
	&& BlasterHUD->GetCharacterOverlay()->DefeatsText;

	if(bHUDValid)
	{
		const FString DefeatsString = FString::Printf(TEXT("Defeats : %d"), Defeats);
		BlasterHUD->GetCharacterOverlay()->DefeatsText->SetText(FText::FromString(DefeatsString));
	}
}

void ABlasterPlayerController::SetHUD_CountdownTime(float CountdownTime) const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetCurrentMatchTimerOverlay() == nullptr) return;

	BlasterHUD->GetCurrentMatchTimerOverlay()->SetCountdownTime(CountdownTime);
}
