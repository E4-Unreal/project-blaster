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

	DOREPLIFETIME(ThisClass, HitScanResults);
}

void AHitScanWeapon::RequestFire(const FVector& HitTarget)
{
	Super::RequestFire(HitTarget);

	// TODO MuzzleFlashSocket 멤버 변수화?
	// Muzzle Location
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	const FVector MuzzleLocation = MuzzleFlashSocket == nullptr
	? GetActorLocation()
	: MuzzleFlashSocket->GetSocketLocation(GetWeaponMesh());

	// Hit Scan On Server. 결과는 Hit Scan Result에 저장됨.
	HandleBulletAction(MuzzleLocation, HitTarget);
}

void AHitScanWeapon::HandleBulletAction_Implementation(const FVector_NetQuantize& MuzzleLocation,
                                            const FVector_NetQuantize& HitTarget)
{
	// Hit Scan
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(Owner);

	HitScanResults.Empty(Pellets);
	for(int i = 0; i < Pellets; i++)
	{
		FVector TraceEnd;
		if(bDisableScatter)
			HitScan(MuzzleLocation, HitTarget, TraceEnd);
		else
			HitScanWithScatter(MuzzleLocation, HitTarget, TraceEnd);
	
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			MuzzleLocation,
			TraceEnd,
			ECC_Visibility,
			CollisionQueryParams
		);
		HitScanResults.Emplace(HitResult);
	}

	// Hit Scan 결과 처리
	for (const FHitResult& HitScanResult : HitScanResults)
	{
		// 데미지 적용
		if(HitScanResult.bBlockingHit)
		{
			UGameplayStatics::ApplyDamage(
				HitScanResult.GetActor(),
				Damage,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
		}

		// For Server Player
		SpawnBeamParticles(HitScanResult);
		SpawnHitEffects(HitScanResult);
	}
}

void AHitScanWeapon::HitScan(const FVector& MuzzleLocation, const FVector& HitTarget, FVector& TraceEnd)
{
	const FVector Offset = HitTarget - MuzzleLocation;
	
	TraceEnd = MuzzleLocation + Offset * 1.25f;
}

void AHitScanWeapon::HitScanWithScatter(const FVector& MuzzleLocation, const FVector& HitTarget, FVector& TraceEnd)
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

void AHitScanWeapon::OnRep_HitScanResult()
{
	// Hit Scan 결과 처리
	for (const FHitResult& HitScanResult : HitScanResults)
	{
		SpawnBeamParticles(HitScanResult);
		SpawnHitEffects(HitScanResult);
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
