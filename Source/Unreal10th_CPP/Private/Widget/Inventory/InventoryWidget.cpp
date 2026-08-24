// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventoryWidget.h"
#include "Widget/Inventory/MoneyPanelWidget.h"
#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/DetailInfoWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryUserInterface.h"
#include "Player/ActionPlayerController.h"


void UInventoryWidget::InitializeInventoryWidget(UInventoryComponent* InInven)
{
	ClearInventoryWidget();
	TargetInventory = InInven;

	if (!TargetInventory.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("TargetInventory가 Valid하지 않습니다."));
		return;
	}

	TargetInventory->OnSlotChanged.BindUObject(this, &UInventoryWidget::RefreshSlotWidget);
	TargetInventory->OnMoneyChanged.AddUObject(this, &UInventoryWidget::RefreshMoneyPanel);

	if (SlotGridPanel)
	{
		int32 ChildCount = SlotGridPanel->GetChildrenCount();
		int32 InvenSize = TargetInventory->GetSize();
		SlotSize = FMath::Min(ChildCount, InvenSize);

		SlotWidgets.Empty(SlotSize);
		for (int i = 0; i < SlotSize; i++)
		{
			if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i)))
			{
				SlotWidget->InitializeSlot(TargetInventory.Get(), i);
				SlotWidget->OnSlotEnter.AddWeakLambda(
					this,
					[this](int InIndex)
					{
						if (TargetInventory.IsValid())
						{
							DetailInfo->Open(TargetInventory->GetSlot(InIndex)->ItemData);
						}
					}
				);
				SlotWidget->OnSlotLeave.AddWeakLambda(
					this,
					[this]()
					{
						DetailInfo->Close();
					}
				);
				SlotWidgets.Add(SlotWidget);
			}
		}
	}
	RefreshInventoryWidget();
}

void UInventoryWidget::ClearInventoryWidget()
{
	SlotWidgets.Empty();
	if (TargetInventory.IsValid())
	{
		TargetInventory->OnSlotChanged.Unbind();
		TargetInventory->OnMoneyChanged.RemoveAll(this);
		TargetInventory = nullptr;
	}
	SlotSize = 0;
}

void UInventoryWidget::OpenInventoryWidget()
{
	SetVisibility(ESlateVisibility::Visible);
	if (AActionPlayerController* PC = Cast<AActionPlayerController>(GetOwningPlayer()))
	{
		PC->OnInventoryOpenClose(true, this);
	}
}

void UInventoryWidget::CloseInventoryWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (AActionPlayerController* PC = Cast<AActionPlayerController>(GetOwningPlayer()))
	{
		PC->OnInventoryOpenClose(false);
	}
}

void UInventoryWidget::ToggleInventoryWidget()
{
	if (IsInventoryOpen())
	{
		CloseInventoryWidget();
	}
	else
	{
		OpenInventoryWidget();
	}
}

void UInventoryWidget::TestRefresh()
{
	RefreshInventoryWidget();
}

void UInventoryWidget::RefreshInventoryWidget() const
{
	if (!TargetInventory.IsValid()) return;

	RefreshMoneyPanel(TargetInventory->GetMoney());

	for (const UInventorySlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->RefreshSlot();
		}
	}
}

void UInventoryWidget::RefreshSlotWidget(int32 InSlotIndex) const
{
	if (IsValidIndex(InSlotIndex) && SlotWidgets[InSlotIndex])
	{
		SlotWidgets[InSlotIndex]->RefreshSlot();
	}
}

void UInventoryWidget::RefreshMoneyPanel(int32 InCurrentMoney) const
{
	if (MoneyPanel)
	{
		MoneyPanel->SetMoney(InCurrentMoney);
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickedCloseButton);
	}

	if (IInventoryUserInterface* InvenUser = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* InvenComp = InvenUser->GetInventoryComponent())
		{
			InitializeInventoryWidget(InvenComp);
		}
	}	

	//CloseInventoryWidget();
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 키가 눌려지면 함수는 무조건 실행
	if (InKeyEvent.GetKey() == EKeys::I)	// 눌려진 키가 I일 때만 처리
	{
		CloseInventoryWidget();
		return FReply::Handled();			// 이 입력에 대한 처리가 끝났다고 알림(Consume처리)
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);	// 내가 처리하지 않은 입력은 부모에서 처리
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Log, TEXT("인벤토리 위젯에서 드랍이 끝났다."));
	FInventoryCommandResult Result;
	UInventoryDragDropOperation* Op = Cast< UInventoryDragDropOperation>(InOperation);
	TargetInventory->ExecuteCommand(FInventoryCommand::MakeMove(TargetInventory->GetTempSlotIndex(), Op->StartIndex), Result);
	return true;
}

void UInventoryWidget::OnClickedCloseButton()
{
	UE_LOG(LogTemp, Log, TEXT("닫기 버튼 눌려짐"));
	CloseInventoryWidget();
}
