// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void RequestFire(const FVector& HitTarget) override;

protected:
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
	/* Hit Scan */
	UFUNCTION(Server, Reliable)
	void HitScan(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& HitTarget);

	void SpawnHitEffects(const FHitResult& HitResult);
	void SpawnBeamParticles(const FHitResult& HitResult);

private:
	/* 탄 퍼짐 */
	UPROPERTY(EditAnywhere)
	bool bDrawDebugScatter;
	
	UPROPERTY(EditAnywhere)
	bool bDisableScatter;
	
	UPROPERTY(EditAnywhere)
	float DistanceToSphere = 800.f;
	
	UPROPERTY(EditAnywhere)
	float SphereRadius = 75.f;
	
	/* 피격 효과 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanResult)
	FHitResult HitScanResult;

	UFUNCTION()
	virtual void OnRep_HitScanResult();
};
