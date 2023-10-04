// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

ABlasterPlayerController::ABlasterPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

