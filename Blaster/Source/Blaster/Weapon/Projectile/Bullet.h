// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitProjectile.h"
#include "Bullet.generated.h"

UCLASS()
class BLASTER_API ABullet : public AHitProjectile
{
	GENERATED_BODY()

protected:
	virtual void ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
