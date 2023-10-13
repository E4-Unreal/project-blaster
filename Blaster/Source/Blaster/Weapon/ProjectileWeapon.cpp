// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile/Projectile.h"

void AProjectileWeapon::ServerFire_Implementation(const FVector_NetQuantize& MuzzleLocation,
	const FVector_NetQuantize& HitTarget)
{
	Super::ServerFire_Implementation(MuzzleLocation, HitTarget);

	// Spawn Bullet
	if(ProjectileClass == nullptr) return;

	if(UWorld* World = GetWorld())
	{
		const FRotator TargetRotation = (HitTarget - MuzzleLocation).Rotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		World->SpawnActor<AProjectile>(
			ProjectileClass,
			MuzzleLocation,
			TargetRotation,
			SpawnParams
		);
	}
}
