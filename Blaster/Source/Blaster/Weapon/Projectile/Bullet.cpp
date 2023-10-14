// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

void ABullet::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// TODO Change to PointDamage
	Super::ApplyDamage(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

