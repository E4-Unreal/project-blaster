// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* 서버, 클라이언트 양쪽 모두 충돌 판정은 활성화함으로써 총알이 장애물을 뚫고 나가는 것을 방지 (Projectile Movement Component)
	 * 대신 OnHit 이벤트 바인딩은 서버에서만 진행함으로써 피격 효과 등은 서버 판정을 따른다.
	 */
	// Collision Box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Projectile Mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Projectile Movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// Trail System Component
	TrailSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailSystemComponent"));
	TrailSystemComponent->SetupAttachment(RootComponent);

	// Tracer Component
	TracerComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TracerComponent"));
	TracerComponent->SetupAttachment(RootComponent);
}

void AProjectile::SpawnHitEffects(const FVector& ImpactPoint, const FVector& ImpactNormal)
{
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactParticles,
			ImpactPoint,
			ImpactNormal.Rotation()
		);
	}

	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			ImpactPoint
		);
	}
}

void AProjectile::SetDestroyTimer()
{
	// Destroy 타이머 설정
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			Destroy();
		}),
		DestroyTime,
		false
	);
}

void AProjectile::Deactivate() const
{
	// 메시 숨기기
	if(ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	
	// 콜리전 비활성화
	if(CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Tracer 비활성화
	if(TracerComponent)
	{
		TracerComponent->Deactivate();
	}

	// Trail 비활성화
	if(TrailSystemComponent)
	{
		TrailSystemComponent->Deactivate();
	}
}
