// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetOwner(AActor* NewOwner) override;

	/* Equip */
	void Equipped();
	void Unequipped();
	void ShowPickupWidget(bool bShowWidget);
	void PlayEquipSound();

	/* Fire */
	virtual void Fire(const FVector& HitTarget);

	/* Reload */
	void AddAmmo(const int32 AmmoAmount);

	/* HUD 크로스헤어 */
	UPROPERTY(EditAnywhere, Category = "Weapon|Crosshairs")
	UTexture2D* CrosshairsCenter;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Crosshairs")
	UTexture2D* CrosshairsLeft;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Crosshairs")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Weapon|Crosshairs")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Weapon|Crosshairs")
	UTexture2D* CrosshairsBottom;

	/* HUD 총알 */
	void InitializeHUD();
	void UpdateHUDAmmo();

	virtual void EnableCollisionAndPhysics(bool Enable);

	/* 커스텀 뎁스 활성화 */
	void EnableCustomDepth(bool bEnable);

protected:
	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	/* Fire */
	UFUNCTION(Server, Reliable)
	virtual void ServerFire(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& HitTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastFire();

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon|Animation")
	UAnimationAsset* FireAnimation;

	// TODO Projectile Weapon 클래스?
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	TSubclassOf<class ACasing> CasingClass;
	
	// 총알
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Weapon|Properties")
	int32 Ammo;

	void SpendRound();
	
	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	int32 MagCapacity;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwningController;

	// 조준 시 줌 인
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float ZoomInterpSpeed = 20.f;

	// 연사 기능
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float FireDelay = .15f;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	bool bIsAutomatic = true;

	// For SetOwner && OnRep_Owner
	void Initialize(AActor* NewOwner);

	UPROPERTY(EditAnywhere, BlueprintGetter = GetWeaponType)
	EWeaponType WeaponType;

	/* Equip */
	UPROPERTY(EditAnywhere, Category = "Weapon|Sound")
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon|Sound")
	float StartTime = 0.f;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	// 조준 시 줌 인
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	// 연사 기능
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }

	// 무기 정보
	UFUNCTION(BlueprintGetter)
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	// Query
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }
	FORCEINLINE bool CanFire() const { return !IsEmpty(); }
	FORCEINLINE bool CanReload() const { return !IsFull(); }
};
