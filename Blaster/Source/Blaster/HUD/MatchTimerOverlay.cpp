// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchTimerOverlay.h"

void UMatchTimerOverlay::SetCountdownTime(float InCountdownTime)
{
	CountdownTime = InCountdownTime >= 0.f ? InCountdownTime : 0.f;
	CountdownMinutes = FMath::FloorToInt(CountdownTime / 60.f);
	CountdownSeconds = FMath::FloorToInt(CountdownTime) % 60;
}
