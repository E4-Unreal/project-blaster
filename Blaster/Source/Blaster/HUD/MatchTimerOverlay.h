// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchTimerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMatchTimerOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CountdownMinutes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CountdownSeconds;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CountdownTime;

public:
	void SetCountdownTime(float InCountdownTime);
};
