// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Money.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"

void UItemAction_Money::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("%s에게 돈 %d만큼 추가"), *InTarget->GetName(), MoneyAmount);
	if (IInventoryUserInterface* InvenUser = Cast<IInventoryUserInterface>(InTarget))
	{
		if (UInventoryComponent* InvenComp = InvenUser->GetInventoryComponent())
		{
			FInventoryCommandResult CommandResult;
			InvenComp->ExecuteCommand(FInventoryCommand::MakeMoney(MoneyAmount), CommandResult);
		}
	}
}
