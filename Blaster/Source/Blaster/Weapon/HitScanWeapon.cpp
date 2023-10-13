// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::ServerFire_Implementation(const FVector_NetQuantize& MuzzleLocation,
	const FVector_NetQuantize& HitTarget)
{
	Super::ServerFire_Implementation(MuzzleLocation, HitTarget);

	// Hit Scan
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(Owner);

	TArray<FHitResult> HitScanResults;
	HitScanResults.Reserve(Pellets);
	
	TMap<AActor*, int32> HitMap;
	HitMap.Reserve(Pellets);
	
	for(int i = 0; i < Pellets; i++)
	{
		// 라인 트레이스
		FVector TraceEnd;
		if(bUseScatter)
			TraceEndWithScatter(MuzzleLocation, HitTarget, TraceEnd);
		else
			TraceEnd = MuzzleLocation + (HitTarget - MuzzleLocation) * 1.25f;
	
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			MuzzleLocation,
			TraceEnd,
			ECC_Visibility,
			CollisionQueryParams
		);
		HitScanResults.Emplace(HitResult);

		// Actor마다 Hit 횟수 카운팅
		if(HitResult.bBlockingHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if(HitMap.Contains(HitActor))
			{
				HitMap[HitActor]++;
			}
			else
			{
				HitMap.Emplace(HitActor, 1);
			}
		}
	}

	// Hit Scan 결과 처리
	MulticastHandleHitResults(HitScanResults);
	for (const auto& HitPair : HitMap)
	{
		UGameplayStatics::ApplyDamage(
				HitPair.Key,
				Damage * HitPair.Value,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
	}
}

void AHitScanWeapon::MulticastHandleHitResults_Implementation(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& HitResult : HitResults)
	{
		SpawnBeamParticles(HitResult);
		SpawnHitEffects(HitResult);
	}
}

void AHitScanWeapon::TraceEndWithScatter(const FVector& MuzzleLocation, const FVector& HitTarget, FVector& TraceEnd)
{
	const FVector Offset = HitTarget - MuzzleLocation;
	FVector Direction = Offset.GetSafeNormal();
	float Distance = Offset.Size();

	// 라인 트레이스
	const FVector SphereCenter = MuzzleLocation + Direction * DistanceToSphere;
	const FVector RandomVector = FMath::VRand() * FMath::FRandRange(0.f, SphereRadius);
	const FVector RandomTarget = SphereCenter + RandomVector;
	TraceEnd = MuzzleLocation + (RandomTarget - MuzzleLocation).GetSafeNormal() * (Distance * 1.25f);

	// 디버그
	if(bDrawDebugScatter)
	{
		DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 10.f);
		DrawDebugSphere(GetWorld(), RandomTarget, 4.f, 12, FColor::Orange, false, 10.f);
	}
}

void AHitScanWeapon::SpawnHitEffects(const FHitResult& HitResult)
{
	if(!HitResult.bBlockingHit) return;
	
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

void AHitScanWeapon::SpawnBeamParticles(const FHitResult& HitResult)
{
	UParticleSystemComponent* Beam =
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			BeamParticles,
			HitResult.TraceStart
		);
	
	if(Beam)
	{
		const FVector Target = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
		Beam->SetVectorParameter(FName("Target"), Target);
	}
}
