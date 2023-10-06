// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
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
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	
	if(ProjectileClass && InstigatorPawn)
	{
		const FRotator TargetRotation = Direction.Rotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
			
		UWorld* World = GetWorld();
		if(World)
		{
			World->SpawnActor<AProjectile>(
				ProjectileClass,
				MuzzleLocation,
				TargetRotation,
				SpawnParams
			);
		}
	}
}
