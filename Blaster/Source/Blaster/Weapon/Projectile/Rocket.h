// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitProjectile.h"
#include "Rocket.generated.h"

UCLASS()
class BLASTER_API ARocket : public AHitProjectile
{
	GENERATED_BODY()

public:
	ARocket();

protected:
	virtual void ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void Deactivate() const override;

private:
	/* Trail */

	/* Sound */
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* InAirLoopAudioComponent;
	
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
