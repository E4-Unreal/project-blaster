// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AProjectileGrenade::AProjectileGrenade()
{
	GetProjectileMovement()->bShouldBounce = true;
}

void AProjectileGrenade::Destroyed()
{
	Super::Destroyed();

	// Spawn Effects
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactParticles,
			GetActorLocation()
		);
	}

	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation()
		);
	}

	// Apply Damage
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

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SetDestroyTimer();
	GetProjectileMovement()->OnProjectileBounce.AddDynamic(this, &ThisClass::OnProjectileBounce);
}

void AProjectileGrenade::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if(BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			ImpactResult.ImpactPoint
		);
	}
}
