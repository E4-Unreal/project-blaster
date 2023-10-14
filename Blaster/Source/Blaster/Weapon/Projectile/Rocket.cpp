// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


ARocket::ARocket()
{
	// In Air Loop Audio Component
	InAirLoopAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("InAirLoopAudioComponent"));
	InAirLoopAudioComponent->SetupAttachment(RootComponent);
}

void ARocket::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor,
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

void ARocket::Deactivate() const
{
	Super::Deactivate();

	// Loop Sound 중지
	if(InAirLoopAudioComponent && InAirLoopAudioComponent->IsPlaying())
	{
		InAirLoopAudioComponent->Stop();
	}
}
