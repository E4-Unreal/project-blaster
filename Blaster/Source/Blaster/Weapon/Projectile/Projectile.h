// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UNiagaraComponent;

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	/* Damage */
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	/* 피격 효과 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UFUNCTION()
	void SpawnHitEffects(const FVector& ImpactPoint, const FVector& ImpactNormal = FVector::ZeroVector);

	/* Destroy */
	virtual void Deactivate() const;

	void SetDestroyTimer();

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* TracerComponent;

	/* Destroy Timer */
	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

protected:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE UNiagaraComponent* GetTrailSystem() const { return TrailSystemComponent; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }

public:
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
};
