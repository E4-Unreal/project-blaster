// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "HitProjectile.generated.h"

UCLASS()
class BLASTER_API AHitProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	/* Hit */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeactivate(const FHitResult& ServerHitResult);

	UFUNCTION()
	virtual void ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
