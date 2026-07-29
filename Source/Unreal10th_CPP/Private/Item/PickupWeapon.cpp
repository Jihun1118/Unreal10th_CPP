// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupWeapon.h"
#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"

void APickupWeapon::OnPickup(AActor* InTarget)
{
	Super::OnPickup(InTarget);

	//FActorSpawnParameters SpawnParam;
	//SpawnParam.Owner = InTarget;
	//SpawnParam.Instigator = InTarget->GetInstigator();
	//
	//AWeaponActor* Weapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponType, FTransform::Identity, SpawnParam);
	//Weapon->EquipToTarget(InTarget);

	IWeaponUserInterface::Execute_EqueipWeapon(InTarget, WeaponData);

	Destroy();

	// 이전 무기 해제하기
	// 피봇 조정하기
}
