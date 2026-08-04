// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SubSystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 프로젝트 세팅에서 데이터 읽어오기
	const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
	if (Settings)
	{
		if (!Settings->DamagePopupClass.IsNull())
		{
			DamagePopupClass = Settings->DamagePopupClass.LoadSynchronous();
		}
	}
}

AActor* UObjectPoolSubsystem::Spawn(const FTransform& InTransform)
{
	AActor* Spawned = nullptr;
	if (ReadyActors.Num() > 0)
	{
		Spawned = ReadyActors.Pop();	// 뒤에서 꺼내기
		Spawned->SetActorTransform(InTransform);
		UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), Spawned  ? *Spawned->GetName() : TEXT("None"));
	}
	else
	{
		if (DamagePopupClass && GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);
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

		ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor) return;
	if (!ActiveActors.Contains(InActor)) return;

	UE_LOG(LogTemp, Log, TEXT("Return : %s"), InActor ? *InActor->GetName() : TEXT("None"));

	if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturn(InActor);
	}

	ActiveActors.Remove(InActor);
	ReadyActors.Add(InActor);
}
