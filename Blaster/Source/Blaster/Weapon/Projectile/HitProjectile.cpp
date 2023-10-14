// Fill out your copyright notice in the Description page of Project Settings.


#include "HitProjectile.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

void AHitProjectile::BeginPlay()
{
	Super::BeginPlay();

	/* 서버에서만 충돌 이벤트 설정 */
	if(HasAuthority())
	{
		// 충돌 무시
		if(Owner) GetCollisionBox()->IgnoreActorWhenMoving(GetOwner(), true); // 무기
		if(GetInstigator()) GetCollisionBox()->IgnoreActorWhenMoving(GetInstigator(), true); // 캐릭터

		// OnHit 이벤트 바인딩
		GetCollisionBox()->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AHitProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ApplyDamage(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	SetDestroyTimer();
	
	MulticastDeactivate(Hit);
}

void AHitProjectile::MulticastDeactivate_Implementation(const FHitResult& ServerHitResult)
{
	Deactivate();
	SpawnHitEffects(ServerHitResult.ImpactPoint, ServerHitResult.ImpactNormal);
}

void AHitProjectile::ApplyDamage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							  FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
}