// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailInfoWidget.generated.h"

class UItemDataAsset;
class UImage;
class UTextBlock;
class UCanvasPanelSlot;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UDetailInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Open(const UItemDataAsset* InItemData);
	void Close();

	inline void SetParentPosition(const FVector2D& InPosition) { ParentPosition = InPosition; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateLocation();


protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemPriceText;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescriptionText;
	
private:
	bool bTickEnable = false;
	TWeakObjectPtr<UCanvasPanelSlot> CanvasSlot;
	FVector2D ParentPosition;
};
