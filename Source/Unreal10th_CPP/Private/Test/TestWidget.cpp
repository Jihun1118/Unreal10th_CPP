#include "Test/TestWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Fill out your copyright notice in the Description page of Project Settings.

void UTestWidget::TestImageColorChange(FLinearColor InColor)
{
	TestImage->SetColorAndOpacity(InColor);
}

void UTestWidget::TestTextChange(FText InString)
{
	TestTextBlock->SetText(InString);
}
