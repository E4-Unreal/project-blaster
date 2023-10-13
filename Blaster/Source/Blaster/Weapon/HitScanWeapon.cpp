// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	// TODO MuzzleFlashSocket 멤버 변수화?
	// Muzzle Location
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	const FVector MuzzleLocation = MuzzleFlashSocket == nullptr
	? GetActorLocation()
	: MuzzleFlashSocket->GetSocketLocation(GetWeaponMesh());

	// Hit Scan On Server. 결과는 Hit Scan Result에 저장됨.
	HitScan(MuzzleLocation, HitTarget);
}

void AHitScanWeapon::HitScan_Implementation(const FVector_NetQuantize& MuzzleLocation,
                                            const FVector_NetQuantize& HitTarget)
{
	// 유효성 검사
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	const FVector Offset = HitTarget - MuzzleLocation;
	FVector Direction = Offset.GetSafeNormal();
	float Distance = Offset.Size();

	// 라인 트레이스
	FHitResult HitResult;
	const FVector Start = MuzzleLocation;
	if(!bDisableScatter)
	{
		FVector SphereCenter = Start + Direction * DistanceToSphere;
		FVector RandomVector = FMath::VRand() * FMath::FRandRange(0.f, SphereRadius);
		FVector RandomTarget = SphereCenter + RandomVector;
		Direction = (RandomTarget - Start).GetSafeNormal();

		// 디버그
		if(bDrawDebugScatter)
		{
			DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 10.f);
			DrawDebugSphere(GetWorld(), RandomTarget, 4.f, 12, FColor::Orange, false, 10.f);
		}
	}
	FVector End = Start + Direction * (Distance * 1.25f);

	// 라인 트레이스
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

void AHitScanWeapon::OnRep_HitScanResult()
{
	SpawnBeamParticles(HitScanResult);
	SpawnHitEffects(HitScanResult);
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
