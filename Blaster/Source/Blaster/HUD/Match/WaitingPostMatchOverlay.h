// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingPostMatchOverlay.generated.h"

class UMatchTimerOverlay;

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

public:
	FORCEINLINE UMatchTimerOverlay* GetMatchTimerOverlay() const { return MatchTimerOverlay; }
};
