// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
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

	// 서버에서만 호출할 것
	void EquipWeapon(AWeapon* NewWeapon);
	void DropEquippedWeapon();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* OldWeapon);

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
	USkeletalMeshSocket const* HandSocket;
	
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

	/* Fire */
	FTimerHandle FireTimer;
	
	bool bCanFire = true;
	bool CanFire();
	
	void StartFireTimer();
	void FireTimerFinished();

	// 현재 장착된 무기의 여분 탄약
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	
	void InitializeCarriedAmmo();

	void UpdateHUDCarriedAmmo();
	
public:

protected:
	void SetIsAiming(bool IsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsAiming(bool IsAiming);
};
