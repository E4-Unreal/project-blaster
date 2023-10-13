// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

protected:
	/* Weapon Interface */
	virtual void ServerFire_Implementation(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& HitTarget) override;

private:
	/* Damage */
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
	/* 탄 퍼짐 */
	UPROPERTY(EditAnywhere)
	bool bDrawDebugScatter;
	
	UPROPERTY(EditAnywhere)
	bool bUseScatter = true;
	
	UPROPERTY(EditAnywhere)
	float DistanceToSphere = 800.f;
	
	UPROPERTY(EditAnywhere)
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	int32 Pellets = 1;

	void TraceEndWithScatter(const FVector& MuzzleLocation, const FVector& HitTarget, FVector& TraceEnd);
	
	/* 피격 효과 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleHitResults(const TArray<FHitResult>& HitResults);

	void SpawnHitEffects(const FHitResult& HitResult);
	void SpawnBeamParticles(const FHitResult& HitResult);
};
