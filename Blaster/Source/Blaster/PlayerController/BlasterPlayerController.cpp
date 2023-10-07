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
		ClearHUDWeapon();
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
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD
	&& BlasterHUD->GetCharacterOverlay()
	&& BlasterHUD->GetCharacterOverlay()->AmmoText;

	if(bHUDValid)
	{
		const FString AmmoString = FString::Printf(TEXT("Ammo : %d"), Ammo);
		BlasterHUD->GetCharacterOverlay()->AmmoText->SetText(FText::FromString(AmmoString));
	}
}

void ABlasterPlayerController::SetHUDMagCapacity(int32 MagCapacity)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD
	&& BlasterHUD->GetCharacterOverlay()
	&& BlasterHUD->GetCharacterOverlay()->MagCapacityText;

	if(bHUDValid)
	{
		const FString MagCapacityString = FString::Printf(TEXT("MagCapacity : %d"), MagCapacity);
		BlasterHUD->GetCharacterOverlay()->MagCapacityText->SetText(FText::FromString(MagCapacityString));
	}
}

void ABlasterPlayerController::ClearHUDWeapon()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if(BlasterHUD && BlasterHUD->GetCharacterOverlay())
	{
		if(BlasterHUD->GetCharacterOverlay()->AmmoText)
			BlasterHUD->GetCharacterOverlay()->AmmoText->SetText(FText::GetEmpty());
		if(BlasterHUD->GetCharacterOverlay()->MagCapacityText)
			BlasterHUD->GetCharacterOverlay()->MagCapacityText->SetText(FText::GetEmpty());
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
