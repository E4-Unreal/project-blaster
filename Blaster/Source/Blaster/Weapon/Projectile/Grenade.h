// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Grenade.generated.h"

UCLASS()
class BLASTER_API AGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	AGrenade();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeactivate(const FVector_NetQuantize& ActorLocation);

	UFUNCTION()
	void SetExplosionTimer();

	UFUNCTION()
	void Explode();

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	UPROPERTY(EditAnywhere)
	float ExplosionTime = 2.f;
	
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

	/* Damage */
	UPROPERTY(EditAnywhere)
	float MinimumDamage = 10.f;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float DamageFalloff = 1.f;
};
