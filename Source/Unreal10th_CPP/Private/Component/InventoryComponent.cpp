// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	Slots.SetNum(InventorySize + 1);	// 일반 슬롯 10개 + 임시 슬롯 1개
}

bool UInventoryComponent::ExecuteCommand(const FInventoryCommand& Command)
{
	return false;
}

void UInventoryComponent::AddMoney(int32 InIncome)
{
	Money += InIncome;
}

int32 UInventoryComponent::AddItem(UItemDataAsset* InItemData, int32 InCount)
{
	if (!InItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("InItemData가 null입니다."));
		return InCount;
	}
	if (InCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InCount가 0이하 입니다."));
		return InCount;
	}

	int32 RemainingCount = InCount;
	
	int32 StartIndex = 0;
	// 같은 종류의 아이템이 있는 슬롯을 찾아 최대한 채우기
	while (RemainingCount > 0)	// 남는게 있으면 계속 반복
	{
		// 같은 종류의 아이템이 들어있는 슬롯을 찾아 추가하기
		int32 FoundIndex = FindSlotWithItem(InItemData, StartIndex);
		if (FoundIndex == InventoryFail) break;	// 같은 종류의 아이템이 들어있는 슬롯이 없으면 종료

		// 같은 종류의 아이템이 들어있는 슬롯을 찾았다.
		FInvenSlot& Slot = Slots[FoundIndex];
		int32 AmountToAdd = FMath::Min(Slot.GetRemainingCount(), RemainingCount);
		UpdateSlotCount(FoundIndex, AmountToAdd);	// FoundIndex 슬롯에 채울 수 있는 만큼 채우기

		RemainingCount -= AmountToAdd;	// 남은 개수 갱신
		StartIndex = FoundIndex + 1;	// 새 시작 위치 갱신
	}

	// 빈슬롯을 찾아 최대한 채우기
	while (RemainingCount > 0)
	{
		int32 EmptyIndex = FindEmptySlot();
		if (EmptyIndex == InventoryFail) break;	// 빈슬롯이 없으면 종료

		FInvenSlot& Slot = Slots[EmptyIndex];
		int32 AmountToAdd = FMath::Min(InItemData->MaxStackCount, RemainingCount);
		SetSlot(EmptyIndex, InItemData, AmountToAdd);	// EmptyIndex 슬롯에 아이템 설정

		RemainingCount -= AmountToAdd;	// 남은 개수 갱신
	}

	// RemainingCount가 0이면 인벤토리에 잘 들어감. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
	return RemainingCount;	
}

void UInventoryComponent::UseItem(int32 InIndex)
{
}

FInvenSlot* UInventoryComponent::GetSlot(int InSlotIndex)
{
	//if (!IsValidIndex(InSlotIndex)) return nullptr;
	check(IsValidIndex(InSlotIndex));

	// check(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 안됨
	// verify(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 됨
	// ensure(bool) : bool이 거짓이면 로그 출력 후 계속. shipping 빌드에 포함이 됨

	return &Slots[InSlotIndex];
}

FInvenSlot* UInventoryComponent::GetTempSlot()
{
	return &Slots[InventorySize];	// 무조건 마지막 슬롯이 Temp슬롯
}

void UInventoryComponent::SetSlot(int32 InSlotIndex, UItemDataAsset* InItemData, int32 InCount)
{
	if (!IsValidIndex(InSlotIndex)) return;

	FInvenSlot& Slot = Slots[InSlotIndex];
	Slot.ItemData = InItemData;
	Slot.SetCount(InCount);

	// 델리게이트 전담 함수
}

void UInventoryComponent::UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount)
{
	if (!IsValidIndex(InSlotIndex)) return;

	FInvenSlot& Slot = Slots[InSlotIndex];
	if (Slot.IsEmpty()) return;
	
	int32 NewCount = Slot.GetCount() + InDeltaCount;
	SetSlot(InSlotIndex, Slot.ItemData, NewCount);	
}

void UInventoryComponent::ClearSlot(int32 InSlotIndex)
{
	SetSlot(InSlotIndex, nullptr, 0);
}


bool UInventoryComponent::HandleAddCommand(const UItemDataAsset* InItemData, int32 InCount)
{
	//if (!InItemData)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("InItemData가 null입니다."));
	//	return InCount;
	//}
	//if (InCount <= 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("InCount가 0이하 입니다."));
	//	return InCount;
	//}

	//int32 RemainingCount = InCount;

	//int32 StartIndex = 0;
	//// 같은 종류의 아이템이 있는 슬롯을 찾아 최대한 채우기
	//while (RemainingCount > 0)	// 남는게 있으면 계속 반복
	//{
	//	// 같은 종류의 아이템이 들어있는 슬롯을 찾아 추가하기
	//	int32 FoundIndex = FindSlotWithItem(InItemData, StartIndex);
	//	if (FoundIndex == InventoryFail) break;	// 같은 종류의 아이템이 들어있는 슬롯이 없으면 종료

	//	// 같은 종류의 아이템이 들어있는 슬롯을 찾았다.
	//	FInvenSlot& Slot = Slots[FoundIndex];
	//	int32 AmountToAdd = FMath::Min(Slot.GetRemainingCount(), RemainingCount);
	//	UpdateSlotCount(FoundIndex, AmountToAdd);	// FoundIndex 슬롯에 채울 수 있는 만큼 채우기

	//	RemainingCount -= AmountToAdd;	// 남은 개수 갱신
	//	StartIndex = FoundIndex + 1;	// 새 시작 위치 갱신
	//}

	//// 빈슬롯을 찾아 최대한 채우기
	//while (RemainingCount > 0)
	//{
	//	int32 EmptyIndex = FindEmptySlot();
	//	if (EmptyIndex == InventoryFail) break;	// 빈슬롯이 없으면 종료

	//	FInvenSlot& Slot = Slots[EmptyIndex];
	//	int32 AmountToAdd = FMath::Min(InItemData->MaxStackCount, RemainingCount);
	//	SetSlot(EmptyIndex, InItemData, AmountToAdd);	// EmptyIndex 슬롯에 아이템 설정

	//	RemainingCount -= AmountToAdd;	// 남은 개수 갱신
	//}

	// RemainingCount가 0이면 인벤토리에 잘 들어감. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
	//return RemainingCount;

	//return RemainingCount == 0;
	return false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UInventoryComponent::FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex)
{
	int32 Result = InventoryFail;

	for (int32 i = InStartIndex; i < InventorySize; i++)
	{
		if (Slots[i].ItemData == InItemData && !Slots[i].IsFull())	// 같은 종류의 아이템인데 스택이 남아있는 경우
		{
			Result = i;
			break;
		}
	}

	return Result;
}

int32 UInventoryComponent::FindEmptySlot()
{
	int32 Result = InventoryFail;
	for (int32 i = 0; i < InventorySize; i++)
	{
		if (Slots[i].IsEmpty())
		{
			Result = i;
			break;
		}
	}

	return Result;
}

