// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// TODO ApplyPointDamage로 대체 예정
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

