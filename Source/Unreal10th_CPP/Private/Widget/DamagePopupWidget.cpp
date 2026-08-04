// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DamagePopupWidget.h"
#include "Components/TextBlock.h"

void UDamagePopupWidget::SetDamage(float InDamage)
{
	DamageText->SetText(FText::AsNumber(static_cast<int32>(InDamage)));
}

void UDamagePopupWidget::PlayPopupAnimation()
{
	PlayAnimation(PopupAnimation);
}
