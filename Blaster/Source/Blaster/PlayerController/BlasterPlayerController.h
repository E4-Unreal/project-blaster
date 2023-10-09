// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	virtual void BeginPlayingState() override;

	/* 서버 - 클라이언트 시간 동기화 */
	virtual void ReceivedPlayer() override;
	virtual float GetServerTime();
	void HandleMatchStates();

protected:
	void UpdateHUD_Countdown();

	/* 서버 - 클라이언트 시간 동기화 */
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaSeconds);

	// 클라이언트와 서버 간의 시간 차이
	float ClientServerDeltaTime = 0.f;

	// 클라이언트에서 서버 시간 요청
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientRequestTime);

	// 서버에서 클라이언트에 서버 시간 응답
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float ClientRequestTime, float ServerReportTime);

	/* Match 상태 */
	UFUNCTION(Server, Reliable)
	void ServerRequestMatchInfo();

	UFUNCTION(Client, Reliable)
	void ClientReportMatchInfo(float ServerLevelStartingTime, float ServerMatchTime, float ServerWarmupTime);

private:
	class ABlasterCharacter* BlasterCharacter;
	class ABlasterHUD* BlasterHUD;
	class ABlasterPlayerState* BlasterPlayerState;
	
	/* Match 상태 */
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	uint32 CountdownTimeInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	// TODO GameMode 이벤트 바인딩?
	void HandleMatchIsWaitingToStart();;
	void HandleMatchHasStarted();

public:
	/* Set HUD */
	void UpdateHUD_All();
	
	// Character
	void SetHUDHealth(float Health);
	void SetHUDMaxHealth(float MaxHealth);

	// Weapon
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDMagCapacity(int32 MagCapacity);
	void HideWeaponOverlay() const;
	void ShowWeaponOverlay() const;

	// Player State
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

	// Match State
	void SetHUD_WarmupCountdown(float CountdownTime) const;
	void SetHUD_MatchCountdown(float CountdownTime) const;
	
	/* Match 상태 */
	void SetMatchState(FName State);
};
