// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxWeight = 100.f;
	FreeWeight = 100.f;
	MaxSlot = 70;
	FreeSlot = 70;
	
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

////////////////////////////////////////////////////////////////////////////
////  

void UInventoryComponent::OnRep_RefreshSlot()
{
	if(OwnerController)
	{
		UE_LOG(LogTemp,Log,TEXT("bRefreshSlot :: RefNotify !"));
		
		OwnerController->ClientUpdateInventory();

		if (bRefreshSlot)
		{
			bRefreshSlot = false;
		}
	}
}

void UInventoryComponent::SetRefreshSlot(bool bRefresh)
{
	bRefreshSlot = bRefresh;

	OnRep_RefreshSlot();
}

void UInventoryComponent::AddItem(FItemTable Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		for (int32 Count = 0; Count < Item.Count; ++Count)
		{
			if (AddInventoryItem(Item))
			{
				SetRefreshSlot(true);
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
		AddInventoryItemAt(Item, SlotIndex);
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
						SetRefreshSlot(true);
					}

					return;
				}
			}
			
			SetRefreshSlot(true);
			return;
		}

		SetRefreshSlot(true);
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
		if(Count > 0)
		{
			RemoveInventoryItemAt(SlotIndex, Count);
		}
	}

	else
	{
		ServerRemoveItemAt(SlotIndex, Count);
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
////  서버 :: 아이템 추가

void UInventoryComponent::ServerAddItem_Implementation(FItemTable Item)
{
	AddItem(Item);

	// MulticastRefreshClients(this);
}
bool UInventoryComponent::ServerAddItem_Validate(FItemTable Item)
{
	return true;
}

void UInventoryComponent::ServerAddItemAt_Implementation(FItemTable Item, int32 SlotIndex)
{
	AddInventoryItemAt(Item, SlotIndex);

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerAddItemAt_Validate(FItemTable Item, int32 SlotIndex)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////
////  서버 :: 아이템 제거

bool UInventoryComponent::ServerRemoveItem_Validate(FItemTable Item)
{
	return true;
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

bool UInventoryComponent::ServerRemoveItemAt_Validate(int32 SlotIndex, int32 Count)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////
////  멀티캐스트 :: 인벤토리 새로고침

bool UInventoryComponent::MulticastRefreshClients_Validate(UInventoryComponent* InvComponent)
{
	return true;
}

void UInventoryComponent::MulticastRefreshClients_Implementation(UInventoryComponent* InvComponent)
{
	InvComponent->SetRefreshSlot(true);

	SetRefreshSlot(true);
}

////////////////////////////////////////////////////////////////////////////
////  인벤토리 기능

bool UInventoryComponent::AddInventoryItem(FItemTable Item)
{
	if (Item.MaxCount > 1) // Stack이 가능한 구조?
	{
		for(int32 Index = 0; Index < Inventory.Num(); ++Index)
		{
			const int32 SlotIndex = GetInventoryIndex(Item.ID, Index);

			if (SlotIndex != -1) // 아이템을 찾았다면?
			{
				if(Inventory[SlotIndex].Count < Inventory[SlotIndex].MaxCount) // MaxCount를 넘지 않았으면?
				{
					Inventory[SlotIndex].Count += 1;
					FreeWeight -= Item.Weight;

					return true;
				}
			}
		}
	}
	
	if(Item.Weight <= FreeWeight && FreeSlot >0)
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
	if(FreeSlot > 0 && Item.Weight <= FreeWeight)
	{
		Inventory.Insert(Item, SlotIndex);

		SetRefreshSlot(true);
	}

	return false;
}

bool UInventoryComponent::RemoveInventoryItem(FItemTable Item)
{
	const int32 SlotIndex = GetInventoryIndex(Item.ID, 0);

	if(SlotIndex != -1) // 아이템이 존재한다면
	{
		if(--Inventory[SlotIndex].Count == 0) // Count가 0이면 삭제
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
	if(Inventory[SlotIndex].Count > Count)
	{
		Inventory[SlotIndex].Count -= Count;
		SetRefreshSlot(true);
		
		return true;
	}

	Inventory.RemoveAt(SlotIndex);
	SetRefreshSlot(true);
	
	return Inventory[SlotIndex].Count == Count;
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
	DOREPLIFETIME(UInventoryComponent, bRefreshSlot);

}