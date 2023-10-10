// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UWaitingPostMatchOverlay;
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
class UWaitingToStartOverlay;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual void DrawHUD() override;

	/* Match State */
	UFUNCTION()
	void OnMatchStateSet(FName NewState);

	void ShowCharacterOverlay();
	void ShowWaitingToStartOverlay();
	void ShowWaitingPostMatchOverlay();
	
	void HideCharacterOverlay();
	void HideWaitingToStartOverlay();
	void HideWaitingPostMatchOverlay();

private:
	/* 사용자 위젯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCharacterOverlay* CharacterOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWaitingToStartOverlay* WaitingToStartOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWaitingPostMatchOverlay* WaitingPostMatchOverlay;

	UPROPERTY(EditAnywhere, Category = Overlay)
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY(EditAnywhere, Category = Overlay)
	TSubclassOf<UUserWidget> WaitingToStartOverlayClass;

	UPROPERTY(EditAnywhere, Category = Overlay)
	TSubclassOf<UUserWidget> WaitingPostMatchOverlayClass;
	
	// 크로스헤어
	FHUDPackage HUDPackage;

	void DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairsSpreadMax = 16.f;

	/* Match State */
	UMatchTimerOverlay* CurrentMatchTimerOverlay;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& InPackage) { HUDPackage = InPackage; }

	// 사용자 위젯 Getter
	FORCEINLINE UCharacterOverlay* GetCharacterOverlay() const { return CharacterOverlay; }
	FORCEINLINE UWaitingToStartOverlay* GetWaitingToStartOverlay() const { return WaitingToStartOverlay; }
	FORCEINLINE UWaitingPostMatchOverlay* GetWaitingPostMatchOverlay() const { return WaitingPostMatchOverlay; }
	
	UWeaponOverlay* GetWeaponOverlay() const;
	UMatchTimerOverlay* GetMatchTimerOverlay() const;

	/* Match State */
	FORCEINLINE UMatchTimerOverlay* GetCurrentMatchTimerOverlay() const { return CurrentMatchTimerOverlay; }
};
