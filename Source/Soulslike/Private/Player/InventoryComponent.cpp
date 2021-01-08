// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryComponent.h"
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

////////////////////////////////////////////////////////////////////////////
//// RepNotify

void UInventoryComponent::OnRep_RefreshSlot()
{
	if(OwnerController)
	{
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

void UInventoryComponent::OnRep_Weapon()
{
	OwnerCharacter->GetWeaponMesh()->SetVisibility(bWeaponEquiped);
}

void UInventoryComponent::SetCurrentWeapon(bool bEquip)
{
	bWeaponEquiped = bEquip;
	
	OnRep_Weapon();	
}


void UInventoryComponent::OnRep_Shield()
{
	// OwnerCharacter->GetShieldMesh()->SetVisibility(bShieldEquiped);
}

void UInventoryComponent::SetCurrentShield(bool bEquip)
{
	bShieldEquiped = bEquip;
	
	OnRep_Shield();
}


////////////////////////////////////////////////////////////////////////////
////

void UInventoryComponent::UseItem(int32 SlotIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (UseInventoryItem(SlotIndex))
		{
			SetRefreshSlot(true);
		}
	}
    
	else
	{
		ServerUseItem(SlotIndex);
	}
}

void UInventoryComponent::UseQuickItem(int32 QuickIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if(UseQuickInventoryItem(QuickIndex))
		{
			SetRefreshSlot(true);
		}
	}

	else
	{
		ServerUseQuickItem(QuickIndex);
	}
}

void UInventoryComponent::UnEquipItem(int32 EquipIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if(UnEquipEquipmentItem(EquipIndex))
		{
			SetRefreshSlot(true);
		}
	}

	else
	{
		ServerUnEquipItem(EquipIndex);
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

void UInventoryComponent::SwapItem(int32 FromIndex, int32 ToIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Inventory.Swap(FromIndex,ToIndex);

		SetRefreshSlot(true);
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
			SetRefreshSlot(true);
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

void UInventoryComponent::SetQuickItemAt(int32 SlotIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		SetQuickInventoryItemAt(SlotIndex);
	}

	else
	{
		ServerSetQuickItemAt(SlotIndex);
	}
}

void UInventoryComponent::SortItem()
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(SortInventoryItem())
		{
			SetRefreshSlot(true);
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

bool UInventoryComponent::ServerUseItem_Validate(int32 SlotIndex)
{
	return true;
}


void UInventoryComponent::ServerUseQuickItem_Implementation(int32 QuickIndex)
{
	UseQuickItem(QuickIndex);
	
	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerUseQuickItem_Validate(int32 QuickIndex)
{
	return true;
}

void UInventoryComponent::ServerUnEquipItem_Implementation(int32 EquipIndex)
{
	UnEquipItem(EquipIndex);

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerUnEquipItem_Validate(int32 EquipIndex)
{
	return true;
}


void UInventoryComponent::ServerAddItem_Implementation(FItemTable Item)
{
	AddItem(Item);

	MulticastRefreshClients(this);
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

void UInventoryComponent::ServerRemoveItem_Implementation(FItemTable Item)
{
	RemoveItem(Item);

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerRemoveItem_Validate(FItemTable Item)
{
	return true;
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

void UInventoryComponent::ServerSwapItem_Implementation(int32 FromIndex, int32 ToIndex)
{
	SwapItem(FromIndex, ToIndex);

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerSwapItem_Validate(int32 FromIndex, int32 ToIndex)
{
	return true;
}


void UInventoryComponent::ServerMoveItem_Implementation(int32 FromIndex, int32 ToIndex)
{
	MoveItem(FromIndex,ToIndex);

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerMoveItem_Validate(int32 FromIndex, int32 ToIndex)
{
	return  true;
}

void UInventoryComponent::ServerLockItemAt_Implementation(int32 SlotIndex)
{
	LockInventoryItemAt(SlotIndex);
}

bool UInventoryComponent::ServerLockItemAt_Validate(int32 SlotIndex)
{
	return true;
}

void UInventoryComponent::ServerSetQuickItemAt_Implementation(int32 SlotIndex)
{
	SetQuickItemAt(SlotIndex);
}

bool UInventoryComponent::ServerSetQuickItemAt_Validate(int32 SlotIndex)
{
	return true;
}

void UInventoryComponent::ServerSortItem_Implementation()
{
	SortItem();

	MulticastRefreshClients(this);
}

bool UInventoryComponent::ServerSortItem_Validate()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////
////  멀티캐스트 :: 인벤토리 새로고침

void UInventoryComponent::MulticastRefreshClients_Implementation(UInventoryComponent* InvComponent)
{
	InvComponent->SetRefreshSlot(true);

	SetRefreshSlot(true);
}

bool UInventoryComponent::MulticastRefreshClients_Validate(UInventoryComponent* InvComponent)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////
////  인벤토리 기능

bool UInventoryComponent::UseInventoryItem(int32 SlotIndex)
{
	if(SlotIndex < Inventory.Num() && Inventory[SlotIndex].Count > 0)
	{
		if(Inventory[SlotIndex].ItemFilter < EItemFilter::Filter_Potion)
		{
			EquipInventoryItem(SlotIndex);
		}
		
		else
		{
			UsePotion(SlotIndex);
		}

		return true;
	}

	return false;
}

bool UInventoryComponent::EquipInventoryItem(int32 SlotIndex)
{
	if(SlotIndex < Inventory.Num())
	{
		const int32 EquipIndex = static_cast<int32>(Inventory[SlotIndex].ItemType);
		if(EquipmentMap.Contains(EquipIndex))
		{
			UnEquipEquipmentItem(EquipIndex);	
		}
	
		EquipmentMap.Emplace(EquipIndex, Inventory[SlotIndex]);
		RemoveItemAt(SlotIndex, 1);

		if(OwnerController) 
		{
			OwnerController->ClientUpdateEquipmentSlot(EquipIndex, EquipmentMap[EquipIndex].Icon);
		
			if(EquipIndex <= 1) // 0:무기, 1:방패만 QuickBar에 등록할 것
			{
				OwnerController->ClientUpdateQuickSlot(EquipIndex+1, EquipmentMap[EquipIndex].Icon, Inventory[EquipIndex].Name);
			}
		}

		if(EquipIndex == 0)
		{
			SetCurrentWeapon(true);
		}

		else if(EquipIndex == 1)
		{
			SetCurrentShield(true);
		}

		return true;
	}

	return false;
}

bool UInventoryComponent::UnEquipEquipmentItem(int32 EquipIndex)
{
	if(EquipmentMap.Contains(EquipIndex))
	{
		AddItem(EquipmentMap[EquipIndex]);
		EquipmentMap.Remove(EquipIndex);
	
		if(OwnerController)
		{
			OwnerController->ClientClearEquipmentSlot(EquipIndex);
		
			if(EquipIndex <= 1) // 0:무기, 1:방패만 QuickBar에 등록할 것
			{
				OwnerController->ClientClearQuickSlot(EquipIndex+1);
			}
		}

		if(EquipIndex == 0)
		{
			SetCurrentWeapon(false);
		}

		else if (EquipIndex == 1)
		{
			SetCurrentShield(false);
		}

		return true;
	}
	
	return false;
}


void UInventoryComponent::UsePotion(int32 SlotIndex)
{
	bool bQuickEmpty = false;;
	if(Inventory[SlotIndex].bQuicked && Inventory[SlotIndex].Count == 1 && Inventory[SlotIndex].ItemFilter == EItemFilter::Filter_Potion)
	{
		bQuickEmpty = true;
	}
	
	RemoveItemAt(SlotIndex, 1);

	if(OwnerController && bQuickEmpty)
	{
		OwnerController->ClientClearQuickSlot(3);
	}
}

bool UInventoryComponent::UseQuickInventoryItem(int32 QuickIndex)
{
	if(QuickSlotMap.Contains(QuickIndex) && QuickSlotMap[QuickIndex] < Inventory.Num())
	{
		const int32 SlotIndex = QuickSlotMap[QuickIndex];
		const int32 Count = Inventory[SlotIndex].Count;
		
		if(SlotIndex < Inventory.Num() && Inventory[SlotIndex].Count > 0)
		{
			// 효과 적용
			
			RemoveItemAt(SlotIndex, 1);
			
			if(OwnerController)
			{
				if(Count == 1)
				{
					OwnerController->ClientClearQuickSlot(QuickIndex);
				}

				else
				{
					OwnerController->ClientUpdateQuickSlot(QuickIndex, Inventory[SlotIndex].Icon, Inventory[SlotIndex].Name);
				}
			}
		
			return true;
		}
	}

	return false;
}

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
	if(Inventory[SlotIndex].Count > Count)
	{
		Inventory[SlotIndex].Count -= Count;
	}

	else // 1=1, 65=65 일 경우
	{
		Inventory.RemoveAt(SlotIndex);
	}
	
	SetRefreshSlot(true);
	return true;
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

bool UInventoryComponent::SetQuickInventoryItemAt(int32 SlotIndex)
{
	if(SlotIndex < Inventory.Num())
	{
		int32 Key = -1;

		switch (Inventory[SlotIndex].ItemFilter)
		{
			case EItemFilter::Filter_Weapon:
				Key=1;
				break;

			case EItemFilter::Filter_Shield:
				Key=2;
				break;
			
			case EItemFilter::Filter_Potion:
				Key=3;
				break;

			default: ;
		}

		if(Key != -1)
		{
			Inventory[SlotIndex].bQuicked=true;
			QuickSlotMap.Add(Key, SlotIndex);

			if(OwnerController)
			{
				OwnerController->ClientUpdateQuickSlot(Key, Inventory[SlotIndex].Icon, Inventory[SlotIndex].Name);
			}
		}
	
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

void UInventoryComponent::GetInventoryItemsByFilter(TArray<FItemTable>& TempInventory, TArray<int32>& TempInvIndex, EItemFilter Filter)
{
	int32 InvIndex=0;
	TempInventory.Empty();

	for(auto& Item : Inventory)
	{
		if(Filter == Item.ItemFilter)
		{
			TempInventory.Add(Item);

			TempInvIndex.Add(InvIndex++);
		}
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
	DOREPLIFETIME(UInventoryComponent, EquipmentMap);
	DOREPLIFETIME(UInventoryComponent, QuickSlotMap);
	DOREPLIFETIME(UInventoryComponent, FreeWeight);
	DOREPLIFETIME(UInventoryComponent, MaxSlot);
	DOREPLIFETIME(UInventoryComponent, FreeSlot);
	DOREPLIFETIME(UInventoryComponent, MaxWeight);
	DOREPLIFETIME(UInventoryComponent, bRefreshSlot);
	DOREPLIFETIME(UInventoryComponent, bWeaponEquiped);
	DOREPLIFETIME(UInventoryComponent, bShieldEquiped);
}