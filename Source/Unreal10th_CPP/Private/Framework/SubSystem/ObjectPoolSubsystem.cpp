// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SubSystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"

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
	}
	else
	{
		if (DamagePopupClass && GetWorld())
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = nullptr;
			SpawnParam.ObjectFlags = RF_Transient;

			Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);
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
		ActiveActors.Add(Spawned);
	}

	return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
	if (!InActor) return;

	ActiveActors.Remove(InActor);
	ReadyActors.Add(InActor);
}
