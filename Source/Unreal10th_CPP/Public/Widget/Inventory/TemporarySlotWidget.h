// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TemporarySlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UTemporarySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetVisual(UTexture2D* InIcon, int32 InCount);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;
	
};
