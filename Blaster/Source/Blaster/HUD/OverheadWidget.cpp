// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(const FString& TextToDisplay)
{
	if(DisplayText)
		DisplayText->SetText(FText::FromString(TextToDisplay));
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	if(const auto PlayerState = InPawn->GetPlayerState())
	{
		SetDisplayText(PlayerState->GetPlayerName());
	}
}

void UOverheadWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
