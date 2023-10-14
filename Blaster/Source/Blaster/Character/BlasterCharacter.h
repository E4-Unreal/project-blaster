// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/Interfaces/IInteractableWithCrosshairs.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

enum class ETurnInPlaceState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdatedSignature, float, Health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMaxHealthUpdatedSignature, float, MaxHealth);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractableWithCrosshairs
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FHealthUpdatedSignature OnHealthUpdated;

	UPROPERTY(BlueprintAssignable)
	FMaxHealthUpdatedSignature OnMaxHealthUpdated;
	
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	/* Equip */
	void Equipped();
	void UnEquipped();

	// 애님 몽타주 재생
	void PlayFireMontage(bool bIsAiming);
	void PlayReloadMontage();
	void PlayEliminatedMontage();

	// Eliminated
	void ServerEliminate();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminate();

	/* HUD */
	void ManualUpdateHUD();

	/* 저격 조준경 */
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void Montage_JumpToSection(FName SectionName) const;

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
	void ReloadButtonPressed();
	
	void AimOffset(float DeltaTime);

	UFUNCTION()
	void OnTakeAnyDamage_Event(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	/* Turn In Place */
	float Yaw;
	float InterpYaw;
	float Pitch;
	FRotator StartingAimRotation;

	ETurnInPlaceState TurnInPlaceState;
	void TurnInPlace(float DeltaTime);

	/* 애님 몽타주 */
 	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* EliminatedMontage;

	void PlayHitReactMontage();

	// 카메라와 캐릭터 간의 거리가 너무 가까워지면 캐릭터 및 무기 메시 숨기기
	void HideCharacter();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	/* 체력 */
 	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, EditAnywhere, Category = PlayerStats)
	float MaxHealth = 100.f;

	UFUNCTION()
	void OnRep_MaxHealth();

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = PlayerStats)
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	/* Eliminated */
	bool bIsEliminated = false;
	FTimerHandle EliminatedTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminatedDelay = 3.f;

	void EliminatedTimerFinished();

	/* ElimBot */
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	/* HUD */
	class ABlasterPlayerController* BlasterPlayerController;

	/* Dissolve Effect */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	// 런타임 때 사용할 다이나믹 머터리얼 인스턴스
	UPROPERTY(VisibleAnywhere, Category = Dissolve)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// 다이나믹 머터리얼 인스턴스의 원본
	UPROPERTY(EditAnywhere, Category = Dissolve)
	UMaterialInstance* DissolveMaterialInstance;
	
public:
	// Character
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Combat Component
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;

	// Turn In Place
	FORCEINLINE float GetYaw() const { return Yaw; }
	FORCEINLINE float GetPitch() const { return Pitch; }
	FORCEINLINE ETurnInPlaceState GetTurnInPlaceState() const { return TurnInPlaceState; }
	FORCEINLINE bool IsEliminated() const { return bIsEliminated; }
};
