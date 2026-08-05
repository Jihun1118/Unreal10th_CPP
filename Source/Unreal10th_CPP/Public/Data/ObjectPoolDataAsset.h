// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObjectPoolDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UObjectPoolDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> ActorClass = nullptr;
};
