// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StatInterface.h"
#include "EnemyCharacter.generated.h"

class UStatComponent;

UCLASS()
class UNREAL10TH_CPP_API AEnemyCharacter : public ACharacter, public IStatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UFUNCTION(BlueprintCallable)
	virtual UStatComponent* GetStatComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	virtual void OnDie();

protected:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDataTable> ItemDropTable;
};
