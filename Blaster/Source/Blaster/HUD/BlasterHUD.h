// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UMatchTimerOverlay;
class UWeaponOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
	
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairsSpread;
	FLinearColor CrosshairsColor;
};

class UCharacterOverlay;
class UAnnouncementOverlay;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	void AddCharacterOverlay();
	void AddAnnouncement();

private:
	// 사용자 위젯
	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	UPROPERTY()
	UAnnouncementOverlay* Announcement;

	UPROPERTY(EditAnywhere, Category = PlayerStats)
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY(EditAnywhere, Category = Announcements)
	TSubclassOf<UUserWidget> AnnouncementClass;
	
	// 크로스헤어
	FHUDPackage HUDPackage;

	void DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairsSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& InPackage) { HUDPackage = InPackage; }

	// 사용자 위젯 Getter
	FORCEINLINE UCharacterOverlay* GetCharacterOverlay() const { return CharacterOverlay; }
	UWeaponOverlay* GetWeaponOverlay() const;
	UMatchTimerOverlay* GetMatchTimerOverlay() const;

	FORCEINLINE UAnnouncementOverlay* GetAnnouncement() const { return Announcement; }
};
