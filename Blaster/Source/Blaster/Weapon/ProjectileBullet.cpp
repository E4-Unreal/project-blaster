// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "Kismet/GameplayStatics.h"

void AProjectileBullet::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
}

