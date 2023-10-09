// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/HUD/Match/WaitingToStartOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/Character/CharacterOverlay.h"
#include "Blaster/HUD/Match/MatchTimerOverlay.h"
#include "Blaster/HUD/Character/WeaponOverlay.h"
#include "Blaster/HUD/Match/WaitingPostMatchOverlay.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 주기적으로 서버 - 클라이언트 시간 동기화
	if(IsLocalController())
	{
		CheckTimeSync(DeltaSeconds);
		UpdateHUD_Countdown();
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
		ServerRequestMatchInfo();
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

void ABlasterPlayerController::ServerRequestMatchInfo_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode)
	{
		SetMatchState(GameMode->GetMatchState());

		// 서버 플레이어 컨트롤러 업데이트
		if(IsLocalController())
			UpdateMatchInfo(GameMode->LevelStartingTime, GameMode->WarmupTime, GameMode->MatchTime, GameMode->CooldownTime);

		// 클라이언트 플레이어 컨트롤러 업데이트
		ClientReportMatchInfo(GameMode->LevelStartingTime, GameMode->WarmupTime, GameMode->MatchTime, GameMode->CooldownTime);
	}
}

void ABlasterPlayerController::ClientReportMatchInfo_Implementation(float ServerLevelStartingTime, float ServerWarmupTime,
                                                                    float ServerMatchTime, float ServerCooldownTime)
{
	UpdateMatchInfo(ServerLevelStartingTime, ServerWarmupTime, ServerMatchTime, ServerCooldownTime);
}

void ABlasterPlayerController::UpdateMatchInfo(float InLevelStartingTime, float InWarmupTime, float InMatchTime, float InCooldownTime)
{
	LevelStartingTime = InLevelStartingTime;
	MatchTime = InMatchTime;
	WarmupTime = InWarmupTime;
	CooldownTime = InCooldownTime;
}

void ABlasterPlayerController::SetMatchState(FName State)
{
	if(MatchState == State) return;
	
	MatchState = State;

	// 서버 플레이어 컨트롤러 설정
	if(!IsLocalController()) return;

	HandleMatchStates();
}

void ABlasterPlayerController::OnRep_MatchState()
{
	HandleMatchStates();
}

void ABlasterPlayerController::HandleMatchStates()
{
	if(MatchState == MatchState::WaitingToStart)
	{
		// TODO APlayerController::ReceivedPlayer가 AGameMode::BeginPlay의 호출 순서로 인한 서버 플레이어 동기화 오류 방지 임시 조치
		ServerRequestMatchInfo();
		HandleMatchIsWaitingToStart();
	}
	else if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldownState();
	}
}

void ABlasterPlayerController::HandleMatchIsWaitingToStart()
{
	if(BlasterHUD)
	{
		BlasterHUD->AddWaitingToStartOverlay();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	if(BlasterHUD)
	{
		BlasterHUD->RemoveWaitingToStartOverlay();
		BlasterHUD->AddCharacterOverlay();
		UpdateHUD_All();
	}
}

void ABlasterPlayerController::HandleCooldownState()
{
	if(BlasterHUD)
	{
		BlasterHUD->RemoveCharacterOverlay();
		BlasterHUD->AddWaitingPostMatchOverlay();
	}
}

/* HUD */

void ABlasterPlayerController::UpdateHUD_All()
{
	// BlasterPlayerController
	UpdateHUD_Countdown();
	
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

void ABlasterPlayerController::UpdateHUD_Countdown()
{
	// TODO 리팩토링
	// 매치 상태에 따른 남은 시간 계산
	float CountdownTime = 0.f;
	if(MatchState == MatchState::WaitingToStart)
		CountdownTime = WarmupTime - (GetServerTime() - LevelStartingTime);
	else if(MatchState == MatchState::InProgress)
		CountdownTime = MatchTime + WarmupTime - (GetServerTime() - LevelStartingTime);
	else if(MatchState == MatchState::Cooldown)
		CountdownTime = CooldownTime + MatchTime + WarmupTime - (GetServerTime() - LevelStartingTime);
	
	const uint32 SecondsLeftInt = FMath::FloorToInt(CountdownTime);
	
	if(CountdownTimeInt != SecondsLeftInt)
	{
		CountdownTimeInt = SecondsLeftInt;

		// TODO BlasterHUD에 Current Overlay 변수로 사용하는 건 어떨까?
		// 매치 상태에 따라 사용중인 사용자 위젯에 남은 시간 업데이트
		if(MatchState == MatchState::WaitingToStart)
			SetHUD_WarmupCountdown(CountdownTime);
		else if(MatchState == MatchState::InProgress)
			SetHUD_MatchCountdown(CountdownTime);
		else if(MatchState == MatchState::Cooldown)
			SetHUD_CooldownCountdown(CountdownTime);
	}
}

void ABlasterPlayerController::SetHUD_WarmupCountdown(float CountdownTime) const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWaitingToStartOverlay() == nullptr || BlasterHUD->GetWaitingToStartOverlay()->GetMatchTimerOverlay() == nullptr) return;

	BlasterHUD->GetWaitingToStartOverlay()->GetMatchTimerOverlay()->SetCountdownTime(CountdownTime);
}

void ABlasterPlayerController::SetHUD_MatchCountdown(float CountdownTime) const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetMatchTimerOverlay() == nullptr) return;

	BlasterHUD->GetMatchTimerOverlay()->SetCountdownTime(CountdownTime);
}

void ABlasterPlayerController::SetHUD_CooldownCountdown(float CountdownTime) const
{
	if(BlasterHUD == nullptr || BlasterHUD->GetWaitingPostMatchOverlay() == nullptr || BlasterHUD->GetWaitingPostMatchOverlay()->GetMatchTimerOverlay() == nullptr) return;

	BlasterHUD->GetWaitingPostMatchOverlay()->GetMatchTimerOverlay()->SetCountdownTime(CountdownTime);
}
