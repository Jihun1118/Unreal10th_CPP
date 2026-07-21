// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputactionValue.h"
#include "ActionPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API AActionPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Look = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPicthMax = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPicthMin = -40;

private:
	void OnLookInput(const FInputActionValue& InValue);

private:
	// 입력 우선 순위
	int32 GameInputPriority = 1;
};
