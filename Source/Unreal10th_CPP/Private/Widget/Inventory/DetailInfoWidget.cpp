// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/DetailInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Data/Item/ItemDataAsset.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UDetailInfoWidget::Open(const UItemDataAsset* InItemData)
{
	if (!InItemData) return;

	ItemIcon->SetBrushFromTexture(InItemData->Icon.Get());
	ItemNameText->SetText(InItemData->DisplayName);
	ItemPriceText->SetText(FText::AsNumber(InItemData->Price));
	ItemDescriptionText->SetText(InItemData->Description);

	SetVisibility(ESlateVisibility::HitTestInvisible);
	bTickEnable = true;
}

void UDetailInfoWidget::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
	bTickEnable = false;
}

void UDetailInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CanvasSlot = Cast<UCanvasPanelSlot>(Slot);	

	//UPanelWidget* ParentWidget = GetParent();
	//UCanvasPanelSlot* ParentSlot = Cast<UCanvasPanelSlot>(ParentWidget->Slot);
	//ParentPosition = ParentSlot->GetPosition();	

	Close();
}

void UDetailInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//FVector2D Screen = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	//UE_LOG(LogTemp, Log, TEXT("%s"), *Screen.ToString());

	if (!bTickEnable) return;	// 있으나 없으나 같음. 위젯은 안보이면 틱이 돌지 않음.

	UpdateLocation();
}

void UDetailInfoWidget::UpdateLocation()
{
	if (!CanvasSlot.IsValid())
	{
		CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	}

	if (CanvasSlot.IsValid())
	{
		FVector2D Screen = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		CanvasSlot->SetPosition(Screen - ParentPosition);
		//CanvasSlot->SetPosition(Screen - FVector2D(940,312));
	}
}
