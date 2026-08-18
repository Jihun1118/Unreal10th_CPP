// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupBase.h"
#include "Data/Item/WeaponDataAsset.h"
#include "PickupWeapon.generated.h"


/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API APickupWeapon : public APickupBase
{
	GENERATED_BODY()
	
public:
	APickupWeapon();
	virtual void InitializePickup(UItemDataAsset* InData) override;

protected:	
	virtual UMeshComponent* GetMesh() const override { return Mesh; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

private:
	// 이 픽업을 먹었을 때 획득하는 무기 데이터
	TWeakObjectPtr<UWeaponDataAsset> WeaponData = nullptr;
};
