// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::RequestFire(const FVector& HitTarget)
{
	Super::RequestFire(HitTarget);
	
	// Muzzle Location
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	const FVector MuzzleLocation = MuzzleFlashSocket == nullptr
	? GetActorLocation()
	: MuzzleFlashSocket->GetSocketLocation(GetWeaponMesh());

	// Direction
	const FVector Direction = HitTarget - MuzzleLocation;

	// Hit Scan On Server
	HitScan(MuzzleLocation, Direction);
}

void AHitScanWeapon::HitScan_Implementation(const FVector_NetQuantize& MuzzleLocation,
	const FVector_NetQuantize& Direction)
{
	// 유효성 검사
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	// 라인 트레이스
	FHitResult HitResult;
	const FVector Start = MuzzleLocation;
	const FVector End = Start + Direction * 1.25f;
	World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility
	);

	/* 스캔 실패 */
	if(!HitResult.bBlockingHit) return;

	/* 스캔 성공 */
	// 데미지 적용
	UGameplayStatics::ApplyDamage(
			HitResult.GetActor(),
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

	// 피격 효과 스폰
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactParticles,
			HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation()
		);
	}

	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			HitResult.ImpactPoint
		);
	}
}
