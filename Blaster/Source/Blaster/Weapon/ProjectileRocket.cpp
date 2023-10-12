// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"


AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if(GetInstigatorController())
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this,
			Damage,
			MinimumDamage,
			GetActorLocation(),
			DamageInnerRadius,
			DamageOuterRadius,
			DamageFalloff,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			GetInstigatorController()
		);
	}
	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
