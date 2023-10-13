// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"


AProjectileRocket::AProjectileRocket()
{
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
	if(GetTrailSystem())
	{
		GetTrailSystem()->Deactivate();
	}

	// Loop Sound 중지
	if(InAirLoopAudioComponent && InAirLoopAudioComponent->IsPlaying())
	{
		InAirLoopAudioComponent->Stop();
	}
	
	// 메시 숨기기
	if(GetProjectileMesh())
	{
		GetProjectileMesh()->SetVisibility(false);
	}

	Super::HandleDestroy();
}
