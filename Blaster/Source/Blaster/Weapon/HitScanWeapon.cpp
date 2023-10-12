// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HitScanResult);
}

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
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(Owner);
	
	World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionQueryParams
	);

	HitScanResult = HitResult;
	SpawnBeamParticles();

	// 스캔 성공 시
	if(HitScanResult.bBlockingHit)
	{
		SpawnHitEffects();
		
		// 데미지 적용
		UGameplayStatics::ApplyDamage(
				HitResult.GetActor(),
				Damage,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
	}
}

void AHitScanWeapon::OnRep_HitScanResult()
{
	SpawnBeamParticles();
	
	if(HitScanResult.bBlockingHit)
	{
		SpawnHitEffects();
	}
}

void AHitScanWeapon::SpawnHitEffects()
{
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactParticles,
			HitScanResult.ImpactPoint,
			HitScanResult.ImpactNormal.Rotation()
		);
	}

	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			HitScanResult.ImpactPoint
		);
	}
}

void AHitScanWeapon::SpawnBeamParticles()
{
	UParticleSystemComponent* Beam =
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			BeamParticles,
			HitScanResult.TraceStart
		);
	
	if(Beam)
	{
		const FVector Target = HitScanResult.bBlockingHit ? HitScanResult.ImpactPoint : HitScanResult.TraceEnd;
		Beam->SetVectorParameter(FName("Target"), Target);
	}
}
