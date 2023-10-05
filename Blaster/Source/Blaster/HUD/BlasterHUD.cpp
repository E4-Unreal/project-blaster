// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"

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

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
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
