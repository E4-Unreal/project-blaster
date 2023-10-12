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

private:
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
	UFUNCTION(Server, Reliable)
	void HitScan(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& Direction);

	UPROPERTY(ReplicatedUsing = OnRep_HitScanResult)
	FHitResult HitScanResult;

	UFUNCTION()
	void OnRep_HitScanResult();

	// 피격 효과
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	void SpawnHitEffects();
	void SpawnBeamParticles();
};
