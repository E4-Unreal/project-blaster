// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

private:
	class ABlasterHUD* BlasterHUD;
};
