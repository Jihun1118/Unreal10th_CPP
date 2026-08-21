// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/TemporarySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UTemporarySlotWidget::SetVisual(UTexture2D* InIcon, int32 InCount)
{
	IconImage->SetBrushFromTexture(InIcon);
	CountText->SetText(FText::AsNumber(InCount));
}
