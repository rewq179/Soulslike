// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/InventoryComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxWeight = 100.f;
	FreeWeight = MaxWeight;
	MaxSlot = 71;
	FreeSlot = MaxSlot;

	SetIsReplicatedByDefault(true);
}

////////////////////////////////////////////////////////////////////////////
////  초기화

void UInventoryComponent::Initialize()
{
	if (auto const Character = Cast<ASoulCharacter>(GetOwner()))
	{
		OwnerCharacter = Character;

		if (auto const Controller = Cast<ASoulPlayerController>(OwnerCharacter->GetController()))
		{
			OwnerController = Controller;
		}
	}
}

void UInventoryComponent::OnRep_RefreshInventory()
{
	if(OwnerController)
	{
		OwnerController->ClientUpdateInventory();

		if (bRefreshInventory)
		{
			bRefreshInventory = false;
		}
	}
}

void UInventoryComponent::SetRefreshInventory(bool bRefresh)
{
	bRefreshInventory = bRefresh;

	OnRep_RefreshInventory();
}

////////////////////////////////////////////////////////////////////////////
////

void UInventoryComponent::UseItem(int32 SlotIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (UseInventoryItem(SlotIndex))
		{
			SetRefreshInventory(true);
		}
	}
    
	else
	{
		ServerUseItem(SlotIndex);
	}
}

void UInventoryComponent::AddItem(FItemTable Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		for (int32 Count = 0; Count < Item.Count; ++Count)
		{
			if (AddInventoryItem(Item))
			{
				SetRefreshInventory(true);
			}
		}
	}

	else
	{
		ServerAddItem(Item);
	}
}

void UInventoryComponent::AddItemAt(FItemTable Item, int32 SlotIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(AddInventoryItemAt(Item, SlotIndex))
		{
			SetRefreshInventory(true);
		}
	}

	else
	{
		ServerAddItemAt(Item, SlotIndex);
	}
}

void UInventoryComponent::RemoveItem(FItemTable Item)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(Item.Count > 0 && IsEnoughCount(Item)) 
		{
			for (int32 Count = 1; Count < Item.Count; ++Count)
			{
				if (!RemoveInventoryItem(Item))
				{
					if(--Count > 0)
					{
						SetRefreshInventory(true);
					}

					return;
				}
			}
			
			SetRefreshInventory(true);
			
			return;
		}

		SetRefreshInventory(true);
	}

	else
	{
		ServerRemoveItem(Item);
	}
}

void UInventoryComponent::RemoveItemAt(int32 SlotIndex, int32 Count)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(Count > 0 && RemoveInventoryItemAt(SlotIndex, Count))
		{
			SetRefreshInventory(true);
		}
	}

	else
	{
		ServerRemoveItemAt(SlotIndex, Count);
	}
}

void UInventoryComponent::SwapItem(int32 FromIndex, int32 ToIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Inventory.Swap(FromIndex,ToIndex);

		SetRefreshInventory(true);
	}

	else
	{
		ServerSwapItem(FromIndex,ToIndex);		
	}
}

void UInventoryComponent::MoveItem(int32 FromIndex, int32 ToIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(MoveInventoryItem(FromIndex, ToIndex))
		{
			SetRefreshInventory(true);
		}
	}

	else
	{
		ServerMoveItem(FromIndex,ToIndex);		
	}
}

void UInventoryComponent::LockItemAt(int32 SlotIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		LockInventoryItemAt(SlotIndex);
	}

	else
	{
		ServerLockItemAt(SlotIndex);
	}
}


void UInventoryComponent::SortItem()
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(SortInventoryItem())
		{
			SetRefreshInventory(true);
		}
	}

	else
	{
		ServerSortItem();
	}
}

bool UInventoryComponent::IsEnoughCount(FItemTable Item)
{
	int32 Count = Item.Count;

	for(int32 Index = 0; Index < Inventory.Num(); ++Index)
	{
		if(Count <= 0)
		{
			return true;
		}
		
		const int32 SlotIndex = GetInventoryIndex(Item.ID, Index);
		if(SlotIndex != -1 && Count > 0)
		{
			Count-= Inventory[SlotIndex].Count;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////
////  서버 :: 아이템 추가, 제거, 이동

void UInventoryComponent::ServerUseItem_Implementation(int32 SlotIndex)
{
	UseItem(SlotIndex);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerAddItem_Implementation(FItemTable Item)
{
	AddItem(Item);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerAddItemAt_Implementation(FItemTable Item, int32 SlotIndex)
{
	AddInventoryItemAt(Item, SlotIndex);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerRemoveItem_Implementation(FItemTable Item)
{
	RemoveItem(Item);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerRemoveItemAt_Implementation(int32 SlotIndex, int32 Count)
{
	RemoveItemAt(SlotIndex,Count);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerSwapItem_Implementation(int32 FromIndex, int32 ToIndex)
{
	SwapItem(FromIndex, ToIndex);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerMoveItem_Implementation(int32 FromIndex, int32 ToIndex)
{
	MoveItem(FromIndex,ToIndex);

	MulticastRefreshClients(this);
}

void UInventoryComponent::ServerLockItemAt_Implementation(int32 SlotIndex)
{
	LockInventoryItemAt(SlotIndex);
}

void UInventoryComponent::ServerSortItem_Implementation()
{
	SortItem();

	MulticastRefreshClients(this);
}

////////////////////////////////////////////////////////////////////////////
////  멀티캐스트 :: 인벤토리 새로고침

void UInventoryComponent::MulticastRefreshClients_Implementation(UInventoryComponent* InvComponent)
{
	InvComponent->SetRefreshInventory(true);

	SetRefreshInventory(true);
}

////////////////////////////////////////////////////////////////////////////
////  인벤토리 기능

bool UInventoryComponent::UseInventoryItem(int32 SlotIndex)
{
	if(SlotIndex < Inventory.Num() && Inventory[SlotIndex].Count > 0)
	{
		if(Inventory[SlotIndex].ItemFilter< EItemFilter::Filter_Potion)
		{
			OwnerCharacter->AddQuickItemAt(Inventory[SlotIndex], 0);

			RemoveItemAt(SlotIndex, 1);
		}
		
		else
		{
			UsePotion(SlotIndex);
		}

		return true;
	}

	return false;
}

void UInventoryComponent::UsePotion(int32 SlotIndex)
{
	bool bQuickEmpty = false;
	if(Inventory[SlotIndex].bQuicked && Inventory[SlotIndex].Count == 1 && Inventory[SlotIndex].ItemFilter == EItemFilter::Filter_Potion)
	{
		bQuickEmpty = true;
	}
	
	RemoveItemAt(SlotIndex, 1);

	if(OwnerController && bQuickEmpty)
	{
		OwnerController->ClientClearQuickBar(3);
	}
}

bool UInventoryComponent::AddInventoryItem(FItemTable Item)
{
	if (Item.MaxCount > 1) // Stack이 가능한 구조?
	{
		for(int32 Index = 0; Index < Inventory.Num(); ++Index)
		{
			const int32 SlotIndex = GetInventoryIndex(Item.ID, Index);

			if (SlotIndex != -1 && Inventory[SlotIndex].Count < Inventory[SlotIndex].MaxCount) // 아이템이 있고, MaxCount를 넘지 않으면?
			{
				if(Item.Weight <= GetFreeWeight()) // 무게가 적당하면?
				{
					Inventory[SlotIndex].Count += 1;
					FreeWeight -= Item.Weight;

					return true;
				}

				return false;
			}
		}
	}
	
	if(Item.Weight <= GetFreeWeight()  && FreeSlot >0)
	{
		Item.Count = 1;
		Inventory.Add(Item);

		FreeSlot--;
		FreeWeight -= Item.Weight;

		return true;
	}

	return false;
}

bool UInventoryComponent::AddInventoryItemAt(FItemTable Item, int32 SlotIndex)
{
	if(FreeSlot > 0 && Item.Weight <= GetFreeWeight())
	{
		Inventory.Insert(Item, SlotIndex);

		return true;
	}

	return false;
}

bool UInventoryComponent::RemoveInventoryItem(FItemTable Item)
{
	const int32 SlotIndex = GetInventoryIndex(Item.ID, 0);

	if(SlotIndex != -1) // 아이템이 존재한다면
	{
		if(Inventory[SlotIndex].Count == 1) // Count가 0이면 삭제
		{
			Inventory.RemoveAt(SlotIndex);
		}

		FreeWeight += Item.Weight;
		return true;
	}

	return false;
}

bool UInventoryComponent::RemoveInventoryItemAt(int32 SlotIndex, int32 Count)
{
	if(SlotIndex < Inventory.Num())
	{
		if(Inventory[SlotIndex].Count > Count)
		{
			Inventory[SlotIndex].Count -= Count;
		}

		else // 1=1, 65=65 일 경우
		{
			FreeWeight += Inventory[SlotIndex].Weight;
			Inventory.RemoveAt(SlotIndex);
		}
	
		return true;
	}

	return false;
}

bool UInventoryComponent::MoveInventoryItem(int32 FromIndex, int32 ToIndex)
{
	if(ToIndex >= 0)
	{
		Inventory.Emplace(Inventory[FromIndex]);
		Inventory.RemoveAt(FromIndex);

		return true;
	}

	return false;
}


bool UInventoryComponent::LockInventoryItemAt(int32 SlotIndex)
{
	if(Inventory.IsValidIndex(SlotIndex))
	{
		Inventory[SlotIndex].bLocked= !Inventory[SlotIndex].bLocked;

		return true;
	}

	return false;
}

bool UInventoryComponent::SortInventoryItem()
{
	if(Inventory.Num() > 0)
	{
		Algo::SortBy(Inventory, &FItemTable::ID);
		
		return true;
	}

	return false;
}

int32 UInventoryComponent::GetInventoryIndex(int32 Id, int32 StartIndex)
{
	for (int32 SlotIndex = StartIndex; SlotIndex < Inventory.Num(); ++SlotIndex)
	{
		if (Id == Inventory[SlotIndex].ID)
		{
			return SlotIndex;
		}
	}

	return -1;
}

float UInventoryComponent::GetFreeWeight() const
{
	if(OwnerCharacter)
	{
		return FreeWeight - OwnerCharacter->GetEquipWeight();
	}
	
	return -1.f;
}

float UInventoryComponent::GetCurWeight() const
{
	if(OwnerCharacter)
	{
		return MaxWeight - FreeWeight + OwnerCharacter->GetEquipWeight();
    }

	return -1.f;
}

FText UInventoryComponent::GetWeightText()
{
	TArray<FStringFormatArg> args;
	args.Add(FStringFormatArg(static_cast<int32>(GetCurWeight())));
	args.Add(FStringFormatArg(static_cast<int32>(GetMaxWeight())));

	return FText::FromString(FString::Format(TEXT("Weight : {0}/{1}"), args));
}

void UInventoryComponent::HoverInventorySlot(int32 SlotIndex)
{
	if(SlotIndex < Inventory.Num() && OwnerController)
	{
		OwnerController->ClientUpdateItemDescription(Inventory[SlotIndex]);
	}
}

////////////////////////////////////////////////////////////////////////////
////  기타

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, OwnerCharacter);
	DOREPLIFETIME(UInventoryComponent, OwnerController);
	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, FreeWeight);
	DOREPLIFETIME(UInventoryComponent, MaxSlot);
	DOREPLIFETIME(UInventoryComponent, FreeSlot);
	DOREPLIFETIME(UInventoryComponent, MaxWeight);
	DOREPLIFETIME(UInventoryComponent, bRefreshInventory);
}

