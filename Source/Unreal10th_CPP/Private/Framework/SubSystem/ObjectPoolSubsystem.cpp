// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SubSystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"
#include "Data/ObjectPoolDataAsset.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (!Settings) return;

	for (const TSoftObjectPtr<UObjectPoolDataAsset>& DataAsset : Settings->PoolDataAssets)
	{
		if (!DataAsset.IsNull())
		{
			TObjectPtr<UObjectPoolDataAsset> LoadedDataAsset = DataAsset.LoadSynchronous();
			ObjectPools.FindOrAdd(LoadedDataAsset->ActorClass.LoadSynchronous());

		}
	}
}

void UObjectPoolSubsystem::Deinitialize()
{
	ClearAllPools();
	Super::Deinitialize();
}

void UObjectPoolSubsystem::Warmup(TSubclassOf<AActor> InClass)
{
}

void UObjectPoolSubsystem::WarmupAll()
{
}

void UObjectPoolSubsystem::ClearPool(TSubclassOf<AActor> InClass)
{
	if (FObjectPool* Pool = ObjectPools.Find(InClass))
	{
		for (AActor* Actor : Pool->ReadyActors)
		{
			if (IsValid(Actor)) Actor->Destroy();
		}
		Pool->ReadyActors.Empty();
		for (AActor* Actor : Pool->ActiveActors)
		{
			if (IsValid(Actor)) Actor->Destroy();
		}
		Pool->ActiveActors.Empty();
	}
}

void UObjectPoolSubsystem::ClearAllPools()
{
	for (auto& [Key, _] : ObjectPools)
	{
		ClearPool(Key);
	}
	ObjectPools.Empty();
}

AActor* UObjectPoolSubsystem::Spawn(TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
	if (!InClassType) return nullptr;

	FObjectPool* Pool = ObjectPools.Find(InClassType);
	if (!Pool) return nullptr;

	AActor* Spawned = nullptr;
	if (Pool->ReadyActors.Num() > 0)
	{
		Spawned = Pool->ReadyActors.Pop();	// 뒤에서 꺼내기
		Spawned->SetActorTransform(InTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));
	}
	else
	{
		if (GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);
			UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), Spawned ? *Spawned->GetName() : TEXT("None"));
#if WITH_EDITOR
			if (Spawned)
			{
				Spawned->SetFolderPath(FName("Pool"));
			}
#endif
		}
	}

	if (Spawned)
	{
		if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
		{
			//UE_LOG(LogTemp, Log, TEXT("OnSpawn"));
			IPoolableInterface::Execute_OnSpawn(Spawned);
		}
		else
		{
			Spawned->SetActorHiddenInGame(false);
			Spawned->SetActorTickEnabled(true);
			Spawned->SetActorEnableCollision(true);
		}

		Pool->ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor) return;

	TSubclassOf<AActor> ActorClass = InActor->GetClass();
	FObjectPool* Pool = ObjectPools.Find(ActorClass);

	if (!Pool || !Pool->ActiveActors.Contains(InActor)) return;

	UE_LOG(LogTemp, Log, TEXT("Return : %s"), InActor ? *InActor->GetName() : TEXT("None"));

	if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturn(InActor);
	}
	else
	{
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorTickEnabled(false);
		InActor->SetActorEnableCollision(false);
	}

	Pool->ActiveActors.Remove(InActor);
	Pool->ReadyActors.Add(InActor);
}
