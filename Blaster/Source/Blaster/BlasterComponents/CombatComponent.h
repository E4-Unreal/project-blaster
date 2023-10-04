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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// Fire
	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	// HUD
	void SetHUDCrosshairs(float DeltaTime);

private:
	ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	void UpdateIsAiming(bool IsAiming);

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

	// Aiming and FOV
	// TODO 제거?
	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DefaultZoomInterpSpeed = 20.f;
	
	float DefaultFOV;
	float CurrentFOV;

	void InterpFOV(float DeltaTime);
	
public:

protected:
	void SetIsAiming(bool IsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsAiming(bool IsAiming);
};
