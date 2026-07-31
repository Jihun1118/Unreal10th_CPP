// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupWeapon.h"
#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"

#include "Components/SphereComponent.h"

void APickupWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (WeaponData)
	{
		if (USkeletalMesh* MeshData = WeaponData->Mesh.LoadSynchronous())
		{
			Mesh->SetSkeletalMesh(MeshData);
			Mesh->SetRelativeLocation(MeshBaseLocation + WeaponData->LocationOffset);
		}
	}
}

void APickupWeapon::OnPickup(AActor* InTarget)
{
	if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle)) return;	// 타이머가 이미 작동중이면 종료(중복실행 방지)

	Super::OnPickup(InTarget);	
	
	TargetActor = InTarget;

	if (IsPickupEffectAssetReady())	// 에셋이 준비되어 있으면 연출 시작, 없으면 즉시 획득 처리
	{
		// 더 이상의 오버랩이 발생하지 않게 하기
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupStartLocation = Mesh->GetComponentLocation();
		PickupElapsedTime = 0.0f;

		GetWorldTimerManager().SetTimer(
			PickupEffectTimerHandle,
			this,
			&APickupWeapon::OnUpdatePickupEffect,
			TimerInterval,
			true
		);
	}
	else
	{
		OnFinishPickupEffect();
	}
}

void APickupWeapon::OnUpdatePickupEffect()
{
	if (!TargetActor.IsValid())	// 타겟이 살아 있을 때만 진행
	{
		OnFinishPickupEffect();
		return;
	}

	PickupElapsedTime += TimerInterval;
	float Div = FMath::Max(PickupEffectDuration, 0.001f);
	float Progress = PickupElapsedTime / Div;

	float DistanceAlpha = PickupAlpha->GetFloatValue(Progress);
	FVector Goal = TargetActor.Get()->GetActorLocation();
	FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

	float HeightOffset = PickupHeight->GetFloatValue(Progress) * PickupEffecHeight;
	NewLocation.Z += HeightOffset;
	Mesh->SetWorldLocation(NewLocation);

	float Scale = PickupScale->GetFloatValue(Progress);
	Mesh->SetRelativeScale3D(FVector(Scale));

	if (Progress >= 1.0f)
	{
		OnFinishPickupEffect();
	}
}

void APickupWeapon::OnFinishPickupEffect()
{
	GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);
	if (TargetActor.IsValid())
	{
		IWeaponUserInterface::Execute_EquipWeapon(TargetActor.Get(), WeaponData);
	}
	Destroy();
}

bool APickupWeapon::IsPickupEffectAssetReady() const
{
	return PickupAlpha != nullptr && PickupHeight != nullptr && PickupScale != nullptr;
}
