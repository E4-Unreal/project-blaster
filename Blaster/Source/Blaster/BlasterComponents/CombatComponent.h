// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AProjectile;

UENUM(BlueprintType)
enum class EWeaponSocket : uint8
{
	EWS_RightHand UMETA(DisplayName = "RightHand"),
	EWS_LeftHand UMETA(DisplayName = "LeftHand"),

	EWS_MAX UMETA(DisplayName = "Default Max")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedWeaponUpdatedSignature, AWeapon*, EquippedWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	friend class ABlasterCharacter;
	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* HUD */
	UPROPERTY(BlueprintAssignable)
	FEquippedWeaponUpdatedSignature OnEquippedWeaponUpdated;

	void ManualUpdateHUD();

	/* Reload */
	void Reload();
	void AutoReload();
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void AddAmmoToWeapon();

	UFUNCTION(BlueprintPure)
	int32 GetAmountToReload();

	/* Throw Grenade */
	UFUNCTION(BlueprintCallable)
	void FinishThrowingGrenade();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& SpawnLocation, const FVector_NetQuantize& Target);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* OldWeapon);

	/* Crosshairs */
	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	
	/* Fire */
	void FireButtonPressed(bool bPressed);
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

	/* Reload */
	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReload();

	bool CanReload() const;

	/* Throw Grenade */
	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastThrowGrenade();

	void SpawnGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> GrenadeClass;

	AProjectile* SpawnedGrenade;

private:
	ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	/* Equip */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon, BlueprintGetter = GetEquippedWeapon)
	AWeapon* EquippedWeapon;

	void SetEquippedWeapon(AWeapon* NewWeapon);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(AWeapon* NewWeapon);

	void EquipWeapon(AWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerDropWeapon();
	
	void DropWeapon(AWeapon* OldWeapon);

	void AttachWeaponToSocket(AWeapon* Weapon, EWeaponSocket Socket);
	
	/* Aim */
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

	/* Carried Ammo */
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	void SetCarriedAmmo(int32 InCarriedAmmo);

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 60;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 8;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 60;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 16;

	UPROPERTY(EditAnywhere)
	int32 StartingSniperRifleAmmo = 10;

	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 10;
	
	void InitializeCarriedAmmo();

	void UpdateHUDCarriedAmmo();

	/* Combat State */
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState(ECombatState OldState);
	
public:
	UFUNCTION(BlueprintGetter)
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

protected:
	void SetIsAiming(bool IsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsAiming(bool IsAiming);
};
