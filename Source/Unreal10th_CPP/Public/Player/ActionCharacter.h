// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "ActionCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UAnimNotifyState_SectionJump;
class AWeaponActor;
class UWeaponDataAsset;

UCLASS()
class UNREAL10TH_CPP_API AActionCharacter : public ACharacter, public IStatInterface, public IWeaponUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacter();	

	// 무기 장비 관련 함수들
	virtual void EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData) override;
	//void UnEquipWeapon();
	void OnWeaponDrop(UWeaponDataAsset* InDropWeaponData);	// 무기를 다 사용하여 드랍되었을 때 실행될 함수

	// 이벤트 함수
	virtual void OnWeaponAttackState(bool bEnable) override;	// 무기 공격 활성화/비활성화 때 실행되는 함수

	// Getter / Setter들
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual UStatComponent* GetStatComponent() const override;

	virtual FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() override {
		return OnOnWeaponAttackStateChanged;
	};

	void SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJunpNotify);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	void OnTestAction(const FInputActionValue& Value);
	void OnMoveAction(const FInputActionValue& Value);
	void OnAttackAction(const FInputActionValue& Value);
	void OnRollAction(const FInputActionValue& Value);
	void OnSprintStart();
	void OnSprintEnd();

private:
	void SpendSprintStamina(float DeltaTime);

	void SectionJumpForCombo();	// 콤보용으로 섹션 점프하는 함수

	void SpawnWeaponActorAndEquip();

	// 공격 몽타주가 끝났을 때 실행될 함수
	void OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted);

public:
	FOnWeaponAttackStateChanged OnOnWeaponAttackStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Test;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Roll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeed = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float WalkSpeed = 600;

	// 구르기에 필요한 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RollStaminaCost = 30.0f;

	// 달리기에 필요한 초당 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float SprintStaminaCostPerSec = 2.0f;

	// 스태미너 사용 후 자동 회복에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryCoolTime = 3.0f;

	// DEPRECATED : 스태미너가 자동 회복 될 때 초당 회복량
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	//float StaminaAutoRecoveryPerSec = 10.0f;

	// 스태미너가 자동 회복 될 때 타이머 틱당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryPerTick = 1.0f;

	// 스태미너가 자동 회복 될 때 타이머 한 틱의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryInterval = 0.1f;

	// 공격시 소비되는 스테미너 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackCost = 5.0f;

	// 현재 장비 중인 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TWeakObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

	// 현재 장비할 무기의 데이터 에셋(임시 : 무기 관리자로 넘길 예정)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> CurrentWeaponData = nullptr;

	// 기본 무기의 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> DefaultWeaponData = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	bool bSprintMode = false;

	// 발생한 콤보 노티파이를 저장해 놓는 변수
	TWeakObjectPtr<UAnimNotifyState_SectionJump> SectionJumpNotify = nullptr;
	
	// 현재 콤보가 가능한지 확인하기 위한 변수
	bool bComboReady = false;
};
