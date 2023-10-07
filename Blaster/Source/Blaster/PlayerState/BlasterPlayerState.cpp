// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Server
	Initialize();
}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Defeats);
}

void ABlasterPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	// Client
	Initialize();
}

void ABlasterPlayerState::Initialize()
{
	if(Controller == nullptr)
		Controller = Cast<ABlasterPlayerController>(GetOwningController());
	UpdateHUDScore();
	UpdateHUDDefeats();
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	if(!HasAuthority()) return;
	
	SetScore(GetScore() + ScoreAmount);
	UpdateHUDScore();
}

void ABlasterPlayerState::UpdateHUDScore()
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
	UpdateHUDDefeats();
}

void ABlasterPlayerState::UpdateHUDDefeats()
{
	if(Controller)
		Controller->SetHUDDefeats(Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	UpdateHUDScore();
}

void ABlasterPlayerState::OnRep_Defeats()
{
	UpdateHUDDefeats();
}
