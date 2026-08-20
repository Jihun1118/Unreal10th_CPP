// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Widget/Inventory/TemporarySlotWidget.h"
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
	//UE_LOG(LogTemp, Log, TEXT("OnMouseEnter : %d 슬롯"), Index);
	OnSlotEnter.Broadcast(Index);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnSlotLeave.Broadcast();
	//UE_LOG(LogTemp, Log, TEXT("OnMouseLeave : %d 슬롯"), Index);
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UE_LOG(LogTemp, Log, TEXT("드래그가 %d 슬롯에서 시작"), Index);

	UInventoryDragDropOperation* DragOp = NewObject<UInventoryDragDropOperation>();
	//DragOp->ItemData

	UTemporarySlotWidget* DragTempWidget = CreateWidget<UTemporarySlotWidget>(
		this,
		TargetInventory->GetTemporasySlotWidgetClass()
	);
	DragOp->DefaultDragVisual = DragTempWidget;
	OutOperation = DragOp;	// NativeOnDrop과 NativeOnDragCancelled를 발동시키기 위해 필수
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Log, TEXT("드래그가 %d 슬롯에서 종료"), Index);
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Log, TEXT("드래그가 실패"));
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 마우스 버튼이 눌려지면 실행되는 함수
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))	
	{
		// 마우스 왼쪽이 눌려져 있다.
		if (FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index))
		{
			if (!InvenSlot->IsEmpty())
			{
				return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
