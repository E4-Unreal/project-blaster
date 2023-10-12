// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"


AProjectileRocket::AProjectileRocket()
{
	// Rocket Mesh
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Trail System Component
	TrailSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailSystemComponent"));
	TrailSystemComponent->SetupAttachment(RootComponent);

	// In Air Loop Audio Component
	InAirLoopAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("InAirLoopAudioComponent"));
	InAirLoopAudioComponent->SetupAttachment(RootComponent);
}

void AProjectileRocket::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::ApplyRadialDamageWithFalloff(
			this,
			Damage,
			MinimumDamage,
			GetActorLocation(),
			DamageInnerRadius,
			DamageOuterRadius,
			DamageFalloff,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			GetInstigatorController()
		);
}

void AProjectileRocket::HandleDestroy()
{
	// 나이아가라 시스템 비활성화
	if(TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}

	// Loop Sound 중지
	if(InAirLoopAudioComponent && InAirLoopAudioComponent->IsPlaying())
	{
		InAirLoopAudioComponent->Stop();
	}
	
	// 메시 숨기기
	if(RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	
	// 콜리전 비활성화
	if(GetCollisionComponent())
	{
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
