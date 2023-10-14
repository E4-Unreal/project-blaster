// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AGrenade::AGrenade()
{
	GetProjectileMovement()->bShouldBounce = true;
}

void AGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SetExplosionTimer();
	GetProjectileMovement()->OnProjectileBounce.AddDynamic(this, &ThisClass::OnProjectileBounce);
}

void AGrenade::SetExplosionTimer()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&AGrenade::Explode,
		ExplosionTime
	);
}

void AGrenade::Explode()
{
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
	SetDestroyTimer();

	MulticastDeactivate(GetActorLocation());
}

void AGrenade::MulticastDeactivate_Implementation(const FVector_NetQuantize& ActorLocation)
{
	Deactivate();
	SpawnHitEffects(ActorLocation);
}

void AGrenade::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
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
