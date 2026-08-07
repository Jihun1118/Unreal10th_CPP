// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/ItemDataAsset.h"
#include "Engine/AssetManager.h"

TSharedPtr<FStreamableHandle> UItemDataAsset::RequestDataLoad(FStreamableDelegate InDelegate) const
{
	TArray<FSoftObjectPath> TargetsToLoad;
	OnAsyncRequest(TargetsToLoad);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
}

bool UItemDataAsset::IsLoaded() const
{
	// 반드시 별도 구현 필요
	return true;
}

void UItemDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	// 반드시 별도 구현 필요
}

