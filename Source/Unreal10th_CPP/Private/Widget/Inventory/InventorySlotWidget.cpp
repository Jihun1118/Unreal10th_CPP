// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInven, int32 InIndex)
{
	if (!InInven) return;

	TargetInventory = InInven;
	Index = InIndex;

	RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
	if (!TargetInventory.IsValid()) return;

	const FInvenSlot* TargetSlot = TargetInventory->GetSlot(Index);
	if (!TargetSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 null입니다."), Index);
		return;
	}

	if (TargetSlot->IsEmpty())
	{
		// 슬롯이 비어있으면
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetBrushTintColor(FLinearColor::Transparent);
		CountBox->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		// 슬롯이 비어있지 않으면
		IconImage->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
		IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		CountText->SetText(FText::AsNumber(TargetSlot->GetCount()));
		MaxStackText->SetText(FText::AsNumber(TargetSlot->ItemData->MaxStackCount));
		CountBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	UE_LOG(LogTemp, Log, TEXT("OnMouseEnter : %d 슬롯"), Index);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("OnMouseLeave : %d 슬롯"), Index);
	Super::NativeOnMouseLeave(InMouseEvent);
}
