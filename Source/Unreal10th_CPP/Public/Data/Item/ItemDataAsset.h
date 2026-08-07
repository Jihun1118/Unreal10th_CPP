// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class UNREAL10TH_CPP_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSharedPtr<FStreamableHandle> RequestDataLoad(FStreamableDelegate InDelegate) const;
	virtual bool IsLoaded() const;

protected:
	virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
	FText Description;
	
};
