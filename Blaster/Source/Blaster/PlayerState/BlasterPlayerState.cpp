// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/PlayerController/BlasterPlayerController.h"

void ABlasterPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if(Controller == nullptr)
		Controller = Cast<ABlasterPlayerController>(GetOwningController());
	UpdateHUDScore();
}

void ABlasterPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if(Controller == nullptr)
		Controller = Cast<ABlasterPlayerController>(GetOwningController());
	UpdateHUDScore();
}

void ABlasterPlayerState::UpdateHUDScore()
{
	if(Controller)
	{
		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	if(!HasAuthority()) return;
	
	SetScore(GetScore() + ScoreAmount);
	UpdateHUDScore();
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	UpdateHUDScore();
}
