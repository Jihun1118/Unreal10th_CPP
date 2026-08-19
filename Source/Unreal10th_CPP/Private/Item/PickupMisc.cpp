// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupMisc.h"
#include "Data/Item/MiscItemDataAsset.h"

APickupMisc::APickupMisc()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName("NoCollision");
}

void APickupMisc::InitializePickup(const UItemDataAsset* InData)
{
	Super::InitializePickup(InData);

	if (DataAsset)
	{
		MiscData = Cast<const UMiscItemDataAsset>(DataAsset);
		if (UStaticMesh* MeshData = MiscData->Mesh.LoadSynchronous())
		{
			Mesh->SetStaticMesh(MeshData);
			Mesh->SetRelativeLocation(MeshBaseLocation + MiscData->SpawnLocationOffset);
		}
	}
}