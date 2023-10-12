// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Blaster/Blaster.h"
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
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);

	// Projectile Movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HitResult);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// TODO Component로 대체
	if(Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	/* 서버에서만 충돌 이벤트 설정 */
	if(HasAuthority())
	{
		// 충돌 무시
		if(Owner) CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
		if(GetInstigator()) CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);

		// OnHit 이벤트 바인딩
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	HitResult = Hit;
	ApplyDamage(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	SpawnHitEffects();
	HandleDestroy();
}

void AProjectile::OnRep_HitResult()
{
	SpawnHitEffects();
	HandleDestroy();
}

void AProjectile::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	// 자손 클래스에서 설정
}

void AProjectile::SpawnHitEffects()
{
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

void AProjectile::HandleDestroy()
{
	// 콜리전 비활성화
	if(CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Trail 비활성화
	if(TracerComponent)
	{
		TracerComponent->Deactivate();
	}

	// Destroy 타이머 설정
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			Destroy();
		}),
		3.f,
		false
	);
}
