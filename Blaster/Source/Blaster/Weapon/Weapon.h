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

	void Equipped(const USkeletalMeshSocket* InSocket, USkeletalMeshComponent* InMesh);
	void UnEquipped();
	void ShowPickupWidget(bool bShowWidget);

	/* Fire */
	// 클라이언트 측에서 필요한 작업을 마친 다음 서버 RPC 호출
	virtual void RequestFire(const FVector& HitTarget);

	// 멀티캐스트 RPC에서 사용
	virtual void Fire();

	/* Reload */
	void AddAmmo(const int32 AmmoAmount);

	/* HUD 크로스헤어 */
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;
	
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;
	
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	/* HUD 총알 */
	void InitializeHUD();
	void UpdateHUDAmmo();

	virtual void EnableCollisionAndPhysics(bool Enable);

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

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* FireAnimation;

	// TODO Projectile Weapon 클래스?
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
	
	// 총알
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Weapon Properties")
	int32 Ammo;

	void SpendRound();
	
	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 MagCapacity;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwningController;

	// 조준 시 줌 인
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	// 연사 기능
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bIsAutomatic = true;

	// For SetOwner && OnRep_Owner
	void Initialize(AActor* NewOwner);

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	/* Equip */
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = Sound)
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
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	// Query
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }
	FORCEINLINE bool CanFire() const { return !IsEmpty(); }
	FORCEINLINE bool CanReload() const { return !IsFull(); }
};
