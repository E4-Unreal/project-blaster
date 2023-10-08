// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	BlasterCharacter = Cast<ABlasterCharacter>(P);
	if(BlasterCharacter)
		BlasterCharacter->InitializeHUD();
}

void ABlasterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	if(BlasterHUD)
	{
		BlasterHUD->Initialize();
		HideWeaponOverlay();
	}
}

void ABlasterPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if(BlasterPlayerState)
		BlasterPlayerState->Initialize();
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD
	&& BlasterHUD->GetCharacterOverlay()
	&& BlasterHUD->GetCharacterOverlay()->HealthBar
	&& BlasterHUD->GetCharacterOverlay()->HealthText;
	
	if(bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->GetCharacterOverlay()->HealthBar->SetPercent(HealthPercent);

		const FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->GetCharacterOverlay()->HealthText->SetText(FText::FromString(HealthString));
	}
}

void ABlasterPlayerController::SetHUDAmmo(int32 Ammo)
{
	if(BlasterHUD == nullptr) return;

	if(BlasterHUD->GetCharacterOverlay())
	{
		BlasterHUD->GetCharacterOverlay()->SetAmmo(Ammo);
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	if(BlasterHUD == nullptr) return;

	if(BlasterHUD->GetCharacterOverlay())
	{
		BlasterHUD->GetCharacterOverlay()->SetCarriedAmmo(CarriedAmmo);
	}
}

void ABlasterPlayerController::SetHUDMagCapacity(int32 MagCapacity)
{
	if(BlasterHUD == nullptr) return;

	if(BlasterHUD->GetCharacterOverlay())
	{
		BlasterHUD->GetCharacterOverlay()->SetMagCapacity(MagCapacity);
	}
}

void ABlasterPlayerController::HideWeaponOverlay()
{
	if(BlasterHUD == nullptr) return;

	if(UCharacterOverlay* CharacterOverlay = BlasterHUD->GetCharacterOverlay())
	{
		CharacterOverlay->HideWeaponOverlay();
	}
}

void ABlasterPlayerController::ShowWeaponOverlay()
{
	if(BlasterHUD == nullptr) return;

	if(UCharacterOverlay* CharacterOverlay = BlasterHUD->GetCharacterOverlay())
	{
		CharacterOverlay->ShowWeaponOverlay();
	}
}

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
