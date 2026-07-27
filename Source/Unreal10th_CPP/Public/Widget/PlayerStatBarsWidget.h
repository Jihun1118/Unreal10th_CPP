// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatBarsWidget.generated.h"

class UResourceBarWidget;

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UPlayerStatBarsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void InitializePlayerStatBars();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UResourceBarWidget> HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UResourceBarWidget> StaminaBar;
	
};
