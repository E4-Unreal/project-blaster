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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/* Hit */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// TODO Multicast
	UPROPERTY(ReplicatedUsing = OnRep_HitResult)
	FHitResult HitResult;

	UFUNCTION()
	virtual void OnRep_HitResult();
	
	/* Damage */
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	UFUNCTION()
	virtual void ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void SpawnHitEffects();

	/* Destroy Timer */
	UFUNCTION()
	virtual void HandleDestroy();

	void SetDestroyTimer();

	// 피격 효과
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	// TODO 리팩토링
	UParticleSystemComponent* TracerComponent;

	/* Destroy Timer */
	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

protected:
	FORCEINLINE UNiagaraComponent* GetTrailSystem() const { return TrailSystemComponent; }
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }
};
