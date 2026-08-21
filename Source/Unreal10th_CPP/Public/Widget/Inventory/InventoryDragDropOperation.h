// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryDragDropOperation.generated.h"


/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<const UItemDataAsset> ItemData;

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StartIndex = 0;
};
