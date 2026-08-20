// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"

void AActionPlayerController::OnInventoryOpenClose(bool bInventoryOpen, UUserWidget* InFocusWidget)
{
	//FInputModeGameOnly;	// 기본상태
	//FInputModeUIOnly;
	//FInputModeGameAndUI;

	if (bInventoryOpen)
	{
		FInputModeUIOnly InputModeUI;
		InputModeUI.SetWidgetToFocus(InFocusWidget->TakeWidget());

		SetInputMode(InputModeUI);
		SetShowMouseCursor(true);
	}
	else
	{
		FInputModeGameOnly InputModeGame;
		SetInputMode(InputModeGame);
		SetShowMouseCursor(false);
	}
}

void AActionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* SubSystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (SubSystem && DefaultMappingContext)
	{
		SubSystem->AddMappingContext(DefaultMappingContext, GameInputPriority);
	}
	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMax = ViewPicthMax;
		PlayerCameraManager->ViewPitchMin = ViewPicthMin;
	}
}

void AActionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
		
	if (UEnhancedInputComponent* Enhanced = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Enhanced->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AActionPlayerController::OnLookInput);		
	}
}

void AActionPlayerController::OnLookInput(const FInputActionValue& InValue)
{
	FVector2D LookAxis = InValue.Get<FVector2D>();
	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);

	//UE_LOG(LogTemp, Log, TEXT("(%.1f, %.1f)"), LookAxis.X, LookAxis.Y);
}
