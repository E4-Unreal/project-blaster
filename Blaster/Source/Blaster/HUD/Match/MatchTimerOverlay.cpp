// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchTimerOverlay.h"

void UMatchTimerOverlay::SetCountdownTime(float InCountdownTime)
{
	CountdownTime = InCountdownTime >= 0.f ? InCountdownTime : 0.f;
	const int32 Minutes = FMath::FloorToInt(CountdownTime) / 60;
	const int32 Seconds = FMath::FloorToInt(CountdownTime) % 60;
	
	CountdownMinutes = Minutes < 0 ? 0 : Minutes;
	CountdownSeconds = Seconds < 0 ? 0 : Seconds;
}
