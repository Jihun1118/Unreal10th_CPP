// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionCharacter.h"
#include "Component/StatComponent.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Data/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AActionCharacter::AActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraSpringArmComponent->bUsePawnControlRotation = true;	// 스프링암은 컨트롤러 입력에 맞게 회전되기

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	bUseControllerRotationYaw = false;	// 컨트롤러 움직일 때 폰이 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 이동방향으로 바라보게 만들기
}

void AActionCharacter::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
	if (InWeaponData == CurrentWeaponData)
	{
		// 같은 종류의 무기를 먹었다.
		CurrentWeapon->ResetUseCount();
	}
	else
	{
		// 다른 종류의 무기를 장비한다.
		
		// 이전 무기 해제하기
		if (CurrentWeapon.IsValid())
		{
			CurrentWeapon.Get()->DropWeapon();
			CurrentWeapon = nullptr;
		}

		// 새 무기 장비하기
		CurrentWeaponData = InWeaponData;
		if (CurrentWeaponData)	// null일때는 장비 안함
		{
			if (!CurrentWeaponData->IsLoaded())
			{
				// 데이터가 로딩 안되었으면 로딩 요청
				UWeaponDataAsset* RequestedData = CurrentWeaponData;
				CurrentWeaponData->RequestDataLoad(
					FStreamableDelegate::CreateWeakLambda(
						this,
						[this, RequestedData]()
						{
							// 로딩이 완료되면 실행되는 람다 함수					
							if (CurrentWeaponData == RequestedData)
							{
								// 중복으로 로딩 요청했을 때를 대비
								SpawnWeaponActorAndEquip();
							}
						})
				);
			}
			else
			{
				// 로딩 된 상황이면 즉시 스폰하고 장비
				SpawnWeaponActorAndEquip();
			}
		}
	}
}

UStatComponent* AActionCharacter::GetStatComponent() const
{
	//return nullptr;
	return StatComponent;
}

void AActionCharacter::OnWeaponAttackState(bool bEnable)
{
	OnOnWeaponAttackStateChanged.ExecuteIfBound(bEnable);
	//OnOnWeaponAttackStateChanged.Execute(bEnable);
}

void AActionCharacter::OnWeaponDrop(UWeaponDataAsset* InDropWeaponData)
{
	if (DefaultWeaponData && (DefaultWeaponData != InDropWeaponData))
	{
		IWeaponUserInterface::Execute_EquipWeapon(this, DefaultWeaponData);
	}
}

void AActionCharacter::SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJunpNotify)
{
	SectionJumpNotify = InSectionJunpNotify;
	bComboReady = SectionJumpNotify.IsValid();
}

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();		
	}
	if (StatComponent)
	{
		FAutoRecoveryData Data = FAutoRecoveryData(
			StaminaAutoRecoveryCoolTime,
			StaminaAutoRecoveryInterval,
			StaminaAutoRecoveryPerTick);
		StatComponent->InitializeStat(Data);
	}
	if (DefaultWeaponData)
	{
		IWeaponUserInterface::Execute_EquipWeapon(this, DefaultWeaponData);
	}	
}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpendSprintStamina(DeltaTime);
}

void AActionCharacter::SpendSprintStamina(float DeltaTime)
{
	// 달리기 모드이고, 이동하고 있고, 몽타주 재생 중이 아니면
	if (bSprintMode && !GetVelocity().IsNearlyZero() &&
		(AnimInstance && !AnimInstance->IsAnyMontagePlaying()))
	{
		// 스테미너 지속적으로 감소
		if(!IStaminaInterface::Execute_ConsumeStamina(StatComponent, SprintStaminaCostPerSec * DeltaTime))
		{
			OnSprintEnd();	// 스테미너가 다 떨어지면 달리기 모드 정지
		}
	}
}

void AActionCharacter::SectionJumpForCombo()
{
	if (SectionJumpNotify.IsValid() && bComboReady)
	{
		OnAttackEnded(nullptr, true);	// 콤보로 몽타주가 시작되었다 => 이전 애니메이션이 끝났다 => 회수 감소시킨다.
		
		UAnimMontage* Current = AnimInstance->GetCurrentActiveMontage();
		AnimInstance->Montage_SetNextSection(	// 섹션을 변경한다.
			AnimInstance->Montage_GetCurrentSection(Current),	// 이 섹션에서(from)
			SectionJumpNotify->GetNextSectionName(),			// 이 섹션으로 변경(to)
			Current	// 적용할 몽타주
		);
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AActionCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);	// 애니메이션이 끝나는 타이밍에 실행

		OnWeaponAttackState(false);
		IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);		
		bComboReady = false;	// 중복실행 방지
	}
}

void AActionCharacter::SpawnWeaponActorAndEquip()
{
	if (!CurrentWeaponData)
	{
		return;	// 로딩 요청이 끝나기 전에 해제되었을 때를 대비
	}

	CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
		AWeaponActor::StaticClass(), FTransform::Identity, this, this);	// 스폰 시작
	//CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
	//	BaseWeaponType, FTransform::Identity, this, this);	// 스폰 시작

	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon->InitializeWeapon(CurrentWeaponData);
		CurrentWeapon->OnWeaponDrop.BindUObject(this, &AActionCharacter::OnWeaponDrop);
		UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);	// 스폰 완료(=BeginPlay까지 실행)
		CurrentWeapon->EquipToTarget(this);
	}
}

void AActionCharacter::OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("OnAttackEnded"));
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon->Use();
	}
}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveAction);
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AActionCharacter::OnAttackAction);
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionCharacter::OnRollAction);
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Started,
			[this](const FInputActionValue& _) {
				OnSprintStart();
			});
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Completed,
			[this](const FInputActionValue& _) {
				OnSprintEnd();
			});
	}
}

float AActionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UStatComponent* StatComp = GetStatComponent())
	{
		IHealthInterface::Execute_DamageHealth(StatComp, Damage);
		
		UE_LOG(LogTemp, Log, TEXT("%.1f 데미지를 입었습니다. (공격자:%s)"), Damage, 
			EventInstigator ? *EventInstigator->GetName() : TEXT("알 수 없음"));
	}

	return Damage;
}

void AActionCharacter::OnTestAction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));

	//Value.Get<bool>();

}

void AActionCharacter::OnMoveAction(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	FVector WorldDirection = FVector(Input.Y, Input.X, 0).GetSafeNormal();
	
	//UE_LOG(LogTemp, Log, TEXT("Input : %.1f, %.1f"), Input.X, Input.Y);
	//UE_LOG(LogTemp, Log, TEXT("Input : %s"), *Input.ToString());
	//UE_LOG(LogTemp, Log, TEXT("WorldDirection : %.1f, %.1f"), WorldDirection.X, WorldDirection.Y);

	//GetControlRotation(); 컨트롤러의 회전

	// 카메라의 Yaw회전각(Degree)를 Radian으로 변경
	float YawRadian = FMath::DegreesToRadians(GetControlRotation().Yaw);	
	
	// 좌우 회전만 할꺼라 UpVector를 기준축으로 Yaw회전각 만큼 돌리는 회전 만들기
	FQuat ContolYawRotation(FVector::UpVector, YawRadian);					

	// 입력된 방향에 회전 적용(=카메라 Yaw회전 만큼 입력방향을 회전 시키기)
	WorldDirection = ContolYawRotation.RotateVector(WorldDirection);

	AddMovementInput(WorldDirection);
}

void AActionCharacter::OnAttackAction(const FInputActionValue& Value)
{
	if (AnimInstance 
		&& IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > AttackCost
		&& CurrentWeapon.IsValid() && CurrentWeapon->CanUse() )
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 첫번째 콤보 공격
			PlayAnimMontage(AttackMontage);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AActionCharacter::OnAttackEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);

			OnWeaponAttackState(false);
			IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);			
		}
		else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
		{
			SectionJumpForCombo();			
		}
	}
}

void AActionCharacter::OnRollAction(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Log, TEXT("OnRollAction"));
	if (!RollMontage) return;
	
	if (!AnimInstance)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	if (IStaminaInterface::Execute_ConsumeStamina(StatComponent, RollStaminaCost))	// 스테미너 소비 시도 후 소비되면 구르기 실행
	{
		if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
		{
			if (!GetLastMovementInputVector().IsNearlyZero())	// 이동 입력 중이면
			{
				SetActorRotation(GetLastMovementInputVector().Rotation());	// 입력방향으로 즉시 회전해서 구르기
			}

			PlayAnimMontage(RollMontage);
		}
	}
}

void AActionCharacter::OnSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bSprintMode = true;
}

void AActionCharacter::OnSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bSprintMode = false;
}

