// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"

#include "BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if(BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;

	// 애님 인스턴스 변수 업데이트
	Speed = Velocity.Size();
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bIsAiming = BlasterCharacter->IsAiming();
	bIsEliminated = BlasterCharacter->IsEliminated();

	/*
	 * Yaw Offset for Strafing
	 * 급격한 좌우 이동 변경 시 부드러운 애니메이션 전환을 위한 보간
	 * 블렌드 스페이스의 스무딩 시간 변경 시 뒤로 이동하는 경우 애니메이션이 버벅거리므로 코드로 보간
	 */
	const FRotator AimRotation = BlasterCharacter->GetBaseAimRotation(); // World Rotation, -180 ~ 180
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity()); // World Rotation, -180 ~ 180
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	// Leaning
	LastCharacterRotation = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, LastCharacterRotation);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	// Aim Offset
	Yaw = BlasterCharacter->GetYaw();
	Pitch = BlasterCharacter->GetPitch();

	// IK
	if(bIsWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		// FABRIK Effect Transform
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_l"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// 목표를 향해 총구 방향 조정 (오른손 본 트랜스폼 (Rotation) 변경)
		if(BlasterCharacter->IsLocallyControlled())
		{
			bIsLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			FVector RightHandLocation = RightHandTransform.GetLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandLocation, RightHandLocation + (RightHandLocation - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		}
	}

	// Turn In Place
	TurnInPlaceState = BlasterCharacter->GetTurnInPlaceState();
}

void UBlasterAnimInstance::OnEliminated(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	BlasterCharacter->PlayEliminatedMontage();
}
