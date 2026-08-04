// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopupWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetDamage(float InDamage);

	UFUNCTION(BlueprintCallable)
	void PlayPopupAnimation();

protected:
	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAnimation;
};
