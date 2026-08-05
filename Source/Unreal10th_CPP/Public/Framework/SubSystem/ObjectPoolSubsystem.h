// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

// 오브젝트 풀 하나를 나타낼 구조체
USTRUCT()
struct FObjectPool
{
	GENERATED_BODY()

	// 사용 대기 중인 액터들
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ReadyActors;

	// 실제 사용 중인 액터들
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> ActiveActors;
};

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	AActor* Spawn(TSubclassOf<AActor> InClassType, const FTransform& InTransform);

	//template<typename T>
	//T* Spawn(TSubclassOf<T> InClassType, const FTransform& InTransform)
	//{
	//	return Cast<T>(Spawn(TSubclassOf<AActor>(InClassType), InTransform));
	//}

	//template<typename T>
	//T* Spawn(const FTransform& InTransform)
	//{
	//	return Cast<T>(Spawn(T::StaticClass(), InTransform));
	//}

	void ReturnPool(AActor* InActor);

protected:

	UPROPERTY(Transient)
	TMap<const TSubclassOf<AActor>, FObjectPool> ObjectPools;
};
