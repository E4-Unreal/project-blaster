// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Casing.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);

	// Weapon Mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMesh->SetEnableGravity(true);

	WeaponMesh->CustomDepthStencilValue = CUSTOM_DEPTH_BLUE;
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	// AreaSphere
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetSphereRadius(75.f);

	// PickupWidget
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawAtDesiredSize(true);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 초기화
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// 서버에서만 콜리전 활성화
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Ammo);
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	Initialize(NewOwner);

	if(Owner)
		Equipped();
	else
		Unequipped();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	Initialize(Owner);

	if(Owner)
		Equipped();
	else
		Unequipped();
}

void AWeapon::Initialize(AActor* NewOwner)
{
	// 멤버 변수 초기화
	BlasterOwnerCharacter = NewOwner == nullptr ? nullptr : Cast<ABlasterCharacter>(NewOwner);
	BlasterOwningController = BlasterOwnerCharacter == nullptr ? nullptr : Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller);

	// HUD 초기화
	if(BlasterOwningController)
		InitializeHUD();
}

void AWeapon::Equipped()
{
	ShowPickupWidget(false);

	EnableCollisionAndPhysics(false);
	EnableCustomDepth(false);
}

void AWeapon::Unequipped()
{
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	EnableCollisionAndPhysics(true);
	EnableCustomDepth(true);
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::PlayEquipSound()
{
	if(EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation(),
			1.f,
			1.f,
			StartTime
		);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	// TODO MuzzleFlashSocket 멤버 변수화?
	// Muzzle Location
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	const FVector MuzzleLocation = MuzzleFlashSocket == nullptr
	? GetActorLocation()
	: MuzzleFlashSocket->GetSocketLocation(GetWeaponMesh());
	
	ServerFire(MuzzleLocation, HitTarget);
}

void AWeapon::ServerFire_Implementation(const FVector_NetQuantize& MuzzleLocation, const FVector_NetQuantize& HitTarget)
{
	MulticastFire();
}

void AWeapon::MulticastFire_Implementation()
{
	// 무기 애니메이션 재생
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	// 탄피 배출
	if(CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName("AmmoEject");
		if(AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
			
			UWorld* World = GetWorld();
			if(World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}

	// 총알 소비
	SpendRound();
}

void AWeapon::AddAmmo(const int32 AmmoAmount)
{
	if(AmmoAmount <= 0) return;
	
	Ammo = FMath::Clamp(Ammo + AmmoAmount, 0, MagCapacity);
	UpdateHUDAmmo();
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter && BlasterCharacter->GetOverlappingWeapon() == this)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::EnableCollisionAndPhysics(bool Enable)
{
	// Server
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(Enable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	// Server & Client
	if(Enable)
	{
		WeaponMesh->SetSimulatePhysics(true);
	}
	else
	{
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
	}
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if(WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	UpdateHUDAmmo();
}

void AWeapon::OnRep_Ammo()
{
	UpdateHUDAmmo();
}

void AWeapon::InitializeHUD()
{
	if(BlasterOwningController)
	{
		BlasterOwningController->SetHUDAmmo(Ammo);
		BlasterOwningController->SetHUDMagCapacity(MagCapacity);
	}
}

void AWeapon::UpdateHUDAmmo()
{
	if(BlasterOwningController)
	{
		BlasterOwningController->SetHUDAmmo(Ammo);
	}
}
