// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Component/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Unreal10th_CPP/Unreal10th_CPP.h"
#include "Interface/HealthInterface.h"
#include "CommonHeader/ItemDropTable.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UStatComponent* AEnemyCharacter::GetStatComponent() const
{
	return StatComp;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(StatComp))
	{
		StatComp->OnDie.AddDynamic(this, &AEnemyCharacter::OnDie);
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	IHealthInterface::Execute_DamageHealth(StatComp, Damage);

	return Damage;
}

void AEnemyCharacter::OnDie()
{
	UE_LOG(LogTemp, Log, TEXT("%s가 죽었습니다."), *this->GetName());
	//ItemDropTable->GetRowMap;
}
