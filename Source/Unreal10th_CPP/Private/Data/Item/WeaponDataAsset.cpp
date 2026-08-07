// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

bool UWeaponDataAsset::IsLoaded() const
{
	return Super::IsLoaded() && Mesh.IsValid() && TrailVFX.IsValid();
}

void UWeaponDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	Super::OnAsyncRequest(InOutArray);
	InOutArray.Add(Mesh.ToSoftObjectPath());
	InOutArray.Add(TrailVFX.ToSoftObjectPath());
}
