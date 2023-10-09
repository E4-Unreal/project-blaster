// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Defeats);
}

void ABlasterPlayerState::UpdateHUD_All()
{
	if(Controller == nullptr)
		Controller = Cast<ABlasterPlayerController>(GetOwningController());
	UpdateHUD_Score();
	UpdateHUD_Defeats();
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	if(!HasAuthority()) return;
	
	SetScore(GetScore() + ScoreAmount);
	UpdateHUD_Score();
}

void ABlasterPlayerState::UpdateHUD_Score()
{
	if(Controller)
	{
		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	if(!HasAuthority()) return;

	Defeats += DefeatsAmount;
	UpdateHUD_Defeats();
}

void ABlasterPlayerState::UpdateHUD_Defeats()
{
	if(Controller)
		Controller->SetHUDDefeats(Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	UpdateHUD_Score();
}

void ABlasterPlayerState::OnRep_Defeats()
{
	UpdateHUD_Defeats();
}
