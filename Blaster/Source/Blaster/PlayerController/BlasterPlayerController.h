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
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	virtual void BeginPlayingState() override;

	/* Set HUD */
	// Character
	void SetHUDHealth(float Health, float MaxHealth);

	// Weapon
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDMagCapacity(int32 MagCapacity);
	void HideWeaponOverlay();
	void ShowWeaponOverlay();

	// Player State
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

	// Match State
	void SetHUDCountdownTime(float InCountdownTime);

	/* 서버 - 클라이언트 시간 동기화 */
	virtual void ReceivedPlayer() override;
	virtual float GetServerTime();

protected:
	void SetHUDTime();

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

private:
	class ABlasterCharacter* BlasterCharacter;
	class ABlasterHUD* BlasterHUD;
	class ABlasterPlayerState* BlasterPlayerState;

	float MatchTime = 120.f; // TODO GameState로 이동?
	uint32 CountdownTimeInt = 0;
};
