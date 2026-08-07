// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

bool UWeaponDataAsset::IsLoaded() const
{
	return Mesh.IsValid() && TrailVFX.IsValid();
}

void UWeaponDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutArray) const
{
	InOutArray.Add(Mesh.ToSoftObjectPath());
	InOutArray.Add(TrailVFX.ToSoftObjectPath());
}
