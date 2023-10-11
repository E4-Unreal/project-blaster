// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingPostMatchOverlay.generated.h"

class UMatchTimerOverlay;
class ABlasterPlayerState;

UENUM(BlueprintType)
enum class EWinnerState : uint8
{
	EWS_NoOne UMETA(DisplayName = "No One"),
	EWS_LocalPlayer UMETA(DisplayName = "Local Player"),
	EWS_RemotePlayer UMETA(DisplayName = "Remote Player"),

	EWS_MAX UMETA(DisplayName = "Default Max")
};

/**
 * 
 */
UCLASS()
class BLASTER_API UWaitingPostMatchOverlay : public UUserWidget
{
	GENERATED_BODY()

private:
	// 자식 사용자 위젯
	UPROPERTY(meta = (BindWidget))
	UMatchTimerOverlay* MatchTimerOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<ABlasterPlayerState*> TopScoringPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EWinnerState WinnerState;

public:
	FORCEINLINE UMatchTimerOverlay* GetMatchTimerOverlay() const { return MatchTimerOverlay; }
	void SetTopScoringPlayers(const TArray<ABlasterPlayerState*>& InTopScoringPlayers);
};
