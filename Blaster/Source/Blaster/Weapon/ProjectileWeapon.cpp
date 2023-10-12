// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::RequestFire(const FVector& HitTarget)
{
	Super::RequestFire(HitTarget);

	// Muzzle Location
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	const FVector MuzzleLocation = MuzzleFlashSocket == nullptr
	? GetActorLocation()
	: MuzzleFlashSocket->GetSocketLocation(GetWeaponMesh());

	// Direction
	const FVector Direction = HitTarget - MuzzleLocation;

	// Spawn Bullet On Server
	SpawnBullet(MuzzleLocation, Direction);
}

void AProjectileWeapon::SpawnBullet_Implementation(const FVector_NetQuantize& MuzzleLocation,
	const FVector_NetQuantize& Direction)
{
	if(ProjectileClass == nullptr) return;

	if(UWorld* World = GetWorld())
	{
		const FRotator TargetRotation = Direction.Rotation();
		
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
