// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	friend class ABlasterCharacter;
	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// Crosshairs
	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	
	// Fire
	void FireButtonPressed(bool bPressed);
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

private:
	ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	// Aim
	UPROPERTY(Replicated)
	bool bIsAiming;

	void IsAimingUpdated(bool IsAiming);

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bIsFireButtonPressed;
	
	// TODO 레플리케이션
	// Hit Target
	FHitResult HitResult;
	FVector_NetQuantize HitTarget;

	// Crosshairs
	float CrosshairsVelocityFactor;
	float CrosshairsInAirFactor;
	float CrosshairsAimFactor;
	float CrosshairsShootingFactor;
	FHUDPackage HUDPackage;

	// FOV
	// TODO 제거?
	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultZoomInterpSpeed = 20.f;
	
	float DefaultFOV;
	float CurrentFOV;

	void InterpFOV(float DeltaTime);

	// 연사 기능
	FTimerHandle FireTimer;
	
	bool bCanFire = true;
	
	void StartFireTimer();
	void FireTimerFinished();
	
public:

protected:
	void SetIsAiming(bool IsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsAiming(bool IsAiming);
};
