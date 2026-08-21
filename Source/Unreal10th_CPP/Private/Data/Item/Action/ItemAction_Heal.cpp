// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Heal.h"

void UItemAction_Heal::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("%s에게 돈 %.1f만큼 체력 회복"), *InTarget->GetName(), HealAmount);
}
