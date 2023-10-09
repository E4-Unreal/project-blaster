// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/HUD/MatchTimerOverlay.h"
#include "Blaster/HUD/WeaponOverlay.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 주기적으로 서버 - 클라이언트 시간 동기화
	if(IsLocalController())
	{
		CheckTimeSync(DeltaSeconds);
		UpdateCountdownTime();
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	BlasterCharacter = Cast<ABlasterCharacter>(P);
	if(BlasterCharacter)
		BlasterCharacter->UpdateHUD_All();
}

void ABlasterPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if(BlasterPlayerState)
		BlasterPlayerState->UpdateHUD_All();
}

void ABlasterPlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if(TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float ClientRequestTime)
{
	ClientReportServerTime(ClientRequestTime, GetWorld()->GetTimeSeconds());
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float ClientRequestTime, float ServerReportTime)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientRequestTime;
	float CurrentServerTime = ServerReportTime + 0.5f * RoundTripTime;
	ClientServerDeltaTime = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ABlasterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

float ABlasterPlayerController::GetServerTime()
{
	return HasAuthority() ? GetWorld()->GetTimeSeconds() : GetWorld()->GetTimeSeconds() + ClientServerDeltaTime;
}

/* Match 상태 */

void ABlasterPlayerController::SetMatchState(FName State)
{
	MatchState = State;

	if(!IsLocalController()) return;

	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	SetHUD_All();
}

/* HUD */

void ABlasterPlayerController::SetHUD_All()
{
	// HUD 초기화
	if(BlasterHUD)
	{
		BlasterHUD->Initialize();
	}

	if(BlasterCharacter)
	{
		BlasterCharacter->UpdateHUD_All();
	}

	if(BlasterPlayerState)
	{
		BlasterPlayerState->UpdateHUD_All();
	}

	// Hide Overlays
	HideWeaponOverlay();
}

/* Character Overlay */

void ABlasterPlayerController::SetHUDHealth(float Health)
{
	if(BlasterHUD == nullptr || BlasterHUD->GetCharacterOverlay() == nullptr) return;
	
	BlasterHUD->GetCharacterOverlay()->SetHealth(Health);
}

void ABlasterPlayerController::SetHUDMaxHealth(float MaxHealth)
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

/* Match Timer Overlay */

void ABlasterPlayerController::UpdateCountdownTime()
{
	const float SecondsLeft = MatchTime - GetServerTime();
	const uint32 SecondsLeftInt = FMath::FloorToInt(SecondsLeft);

	if(CountdownTimeInt != SecondsLeftInt)
	{
		CountdownTimeInt = SecondsLeftInt;
		SetHUDCountdownTime(SecondsLeft);
	}
}

void ABlasterPlayerController::SetHUDCountdownTime(float InCountdownTime) const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetMatchTimerOverlay() == nullptr) return;

	BlasterHUD->GetMatchTimerOverlay()->SetCountdownTime(InCountdownTime);
}
