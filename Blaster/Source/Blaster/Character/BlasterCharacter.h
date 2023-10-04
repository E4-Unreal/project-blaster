// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Interfaces/IInteractableWithCrosshairs.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

enum class ETurnInPlaceState : uint8;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractableWithCrosshairs
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bIsAiming);

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void Jump() override;
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	
	void AimOffset(float DeltaTime);
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* OldWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float Yaw;
	float InterpYaw;
	float Pitch;
	FRotator StartingAimRotation;

	ETurnInPlaceState TurnInPlaceState;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	// 카메라와 캐릭터 간의 거리가 너무 가까워지면 캐릭터 및 무기 메시 숨기기
	void HideCharacter();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
public:
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Combat Component
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;

	// Turn In Place
	FORCEINLINE float GetYaw() const { return Yaw; }
	FORCEINLINE float GetPitch() const { return Pitch; }
	FORCEINLINE ETurnInPlaceState GetTurnInPlaceState() const { return TurnInPlaceState; }
};
