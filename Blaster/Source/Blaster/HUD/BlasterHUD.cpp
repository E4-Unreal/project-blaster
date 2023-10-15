// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Blaster/GameMode/BlasterGameMode.h"
#include "Match/WaitingToStartOverlay.h"
#include "Character/CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "Character/WeaponOverlay.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Match/MatchTimerOverlay.h"
#include "Match/WaitingPostMatchOverlay.h"

void ABlasterHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(GetOwningPlayerController() == nullptr) return;
	
	CharacterOverlay = CreateWidget<UCharacterOverlay>(GetOwningPlayerController(), CharacterOverlayClass);
	CharacterOverlay->AddToViewport();
	CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	
	WaitingToStartOverlay = CreateWidget<UWaitingToStartOverlay>(GetOwningPlayerController(), WaitingToStartOverlayClass);
	WaitingToStartOverlay->AddToViewport();
	WaitingToStartOverlay->SetVisibility(ESlateVisibility::Hidden);
	
	WaitingPostMatchOverlay = CreateWidget<UWaitingPostMatchOverlay>(GetOwningPlayerController(), WaitingPostMatchOverlayClass);
	WaitingPostMatchOverlay->AddToViewport();
	WaitingPostMatchOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float SpreadScaled = CrosshairsSpreadMax * HUDPackage.CrosshairsSpread;

		// TODO switch 문?
		if(HUDPackage.CrosshairsCenter)
		{
			FVector2D CrosshairsSpread(0.f, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsCenter, ViewportCenter, CrosshairsSpread, HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsLeft)
		{
			FVector2D CrosshairsSpread(-SpreadScaled, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsLeft, ViewportCenter, CrosshairsSpread, HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsRight)
		{
			FVector2D CrosshairsSpread(SpreadScaled, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsRight, ViewportCenter, CrosshairsSpread, HUDPackage.CrosshairsColor);
		}

		// UV 좌표에서는 위쪽 방향이 -Y이다.
		if(HUDPackage.CrosshairsTop)
		{
			FVector2D CrosshairsSpread(0.f, -SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsTop, ViewportCenter, CrosshairsSpread, HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsBottom)
		{
			FVector2D CrosshairsSpread(0.f, SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsBottom, ViewportCenter, CrosshairsSpread, HUDPackage.CrosshairsColor);
		}
	}
}

void ABlasterHUD::OnMatchStateSet(FName NewState)
{
	if(NewState == MatchState::WaitingToStart)
	{
		CurrentMatchTimerOverlay = WaitingToStartOverlay == nullptr ? nullptr : WaitingToStartOverlay->GetMatchTimerOverlay();

		ShowWaitingToStartOverlay();
	}
	else if(NewState == MatchState::InProgress)
	{
		CurrentMatchTimerOverlay = CharacterOverlay == nullptr ? nullptr : CharacterOverlay->GetMatchTimerOverlay();
		
		HideWaitingToStartOverlay();
		ShowCharacterOverlay();
	}
	else if(NewState == MatchState::Cooldown)
	{
		CurrentMatchTimerOverlay = WaitingPostMatchOverlay == nullptr ? nullptr : WaitingPostMatchOverlay->GetMatchTimerOverlay();
		
		HideCharacterOverlay();
		ShowWaitingPostMatchOverlay();
	}
	else if(NewState == MatchState::LeavingMap)
	{
		if(WaitingToStartOverlay)
		{
			CharacterOverlay->RemoveFromParent();
		}
		
		if(CharacterOverlay)
		{
			CharacterOverlay->RemoveFromParent();
		}
		
		if(WaitingPostMatchOverlay)
		{
			WaitingPostMatchOverlay->RemoveFromParent();
		}
	}
}

void ABlasterHUD::ShowCharacterOverlay()
{
	if(CharacterOverlay)
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterHUD::ShowWaitingToStartOverlay()
{
	if(WaitingToStartOverlay)
	{
		WaitingToStartOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterHUD::ShowWaitingPostMatchOverlay()
{
	if(WaitingPostMatchOverlay)
	{
		WaitingPostMatchOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterHUD::HideCharacterOverlay()
{
	if(CharacterOverlay)
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterHUD::HideWaitingToStartOverlay()
{
	if(WaitingToStartOverlay)
	{
		WaitingToStartOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterHUD::HideWaitingPostMatchOverlay()
{
	if(WaitingPostMatchOverlay)
	{
		WaitingPostMatchOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

/* Set Overlay Value */

void ABlasterHUD::SetHealth(float Health)
{
	if(CharacterOverlay)
	{
		CharacterOverlay->SetHealth(Health);
	}
}

void ABlasterHUD::SetMaxHealth(float MaxHealth)
{
	if(CharacterOverlay)
	{
		CharacterOverlay->SetMaxHealth(MaxHealth);
	}
}

void ABlasterHUD::SetScore(int32 Score)
{
	if(CharacterOverlay && CharacterOverlay->ScoreText)
	{
		const FString ScoreString = FString::Printf(TEXT("Score : %d"), Score);
		CharacterOverlay->ScoreText->SetText(FText::FromString(ScoreString));
	}
}

void ABlasterHUD::SetDefeats(int32 Defeats)
{
	if(CharacterOverlay && CharacterOverlay->DefeatsText)
	{
		const FString DefeatsString = FString::Printf(TEXT("Defeats : %d"), Defeats);
		CharacterOverlay->DefeatsText->SetText(FText::FromString(DefeatsString));
	}
}

void ABlasterHUD::SetCountdownTime(float CountdownTime)
{
	if(CurrentMatchTimerOverlay)
	{
		CurrentMatchTimerOverlay->SetCountdownTime(CountdownTime);
	}
}

void ABlasterHUD::SetTopScoringPlayers(const TArray<ABlasterPlayerState*>& InTopScoringPlayers)
{
	if(WaitingPostMatchOverlay)
	{
		WaitingPostMatchOverlay->SetTopScoringPlayers(InTopScoringPlayers);
	}
}

void ABlasterHUD::SetEquippedWeapon(AWeapon* EquippedWeapon)
{
	if(CharacterOverlay == nullptr || CharacterOverlay->GetWeaponOverlay() == nullptr) return;

	CharacterOverlay->GetWeaponOverlay()->SetEquippedWeapon(EquippedWeapon);
}

void ABlasterHUD::SetGrenadeCount(int32 GrenadeCount)
{
	if(CharacterOverlay == nullptr || CharacterOverlay->GetWeaponOverlay() == nullptr) return;

	CharacterOverlay->GetWeaponOverlay()->SetGrenadeCount(GrenadeCount);
}

void ABlasterHUD::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread,
                                 FLinearColor CrosshairsColor)
{
	// TODO ViewportSize에 따른 Texture 크기 조정
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairsColor
	);
}

UWeaponOverlay* ABlasterHUD::GetWeaponOverlay() const
{
	return CharacterOverlay == nullptr ? nullptr : CharacterOverlay->GetWeaponOverlay();
}

UMatchTimerOverlay* ABlasterHUD::GetMatchTimerOverlay() const
{
	return CharacterOverlay == nullptr ? nullptr : CharacterOverlay->GetMatchTimerOverlay();
}
