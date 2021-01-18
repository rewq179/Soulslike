// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/EquipmentComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "Player/Weapon.h"
#include "Player/PlayerAnimInstance.h"

#include "Engine/World.h"
#include "Animation/AnimInstance.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

////////////////////////////////////////////////////////////////////////////
////  초기화

void UEquipmentComponent::Initialize()
{
	if (auto const Character = Cast<ASoulCharacter>(GetOwner()))
	{
		OwnerCharacter = Character;

		if (auto const Controller = Cast<ASoulPlayerController>(OwnerCharacter->GetController()))
		{
			OwnerController = Controller;
		}

		CurrentWeapon = (GetWorld()->SpawnActor<AWeapon>(WeaponClass));
		if(CurrentWeapon)
		{
			CurrentWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon_R_Socket");
			CurrentWeapon->SetHidden(true);
		}
	}
}


void UEquipmentComponent::OnRep_RefreshEquipments()
{
	if(OwnerController)
	{
		OwnerController->ClientUpdateEquipmentSlot();

		if(bRefeshEquipments)
		{
			bRefeshEquipments = false;
		}
	}
}

void UEquipmentComponent::SetRefreshEquipments(bool bRefresh)
{
	bRefeshEquipments=bRefresh;
	
	OnRep_RefreshEquipments();
}


void UEquipmentComponent::OnRep_Weapon()
{
	if(OwnerCharacter)
	{
		OwnerCharacter->SetEquip(!CurrentWeapon->IsHidden());
	}
}

void UEquipmentComponent::SetCurrentWeapon(bool bEquip)
{
	if(CurrentWeapon == nullptr)
	{
		return;
	}
	
	CurrentWeapon->SetHidden(!bEquip);
	
	OnRep_Weapon();
}

////////////////////////////////////////////////////////////////////////////
////

void UEquipmentComponent::UseQuickItem()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if(UseQuickInventoryItem())
		{
			
		}
	}

	else
	{
		ServerUseQuickItem();
	}
}

void UEquipmentComponent::EquipItem(FItemTable Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		EquipInventoryItem(Item);
	}

	else
	{
		ServerEquipItem(Item);
	}
}

void UEquipmentComponent::UnEquipItem(EItemFilter ItemFilter, int32 EquipIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		UnEquipEquipmentItem(ItemFilter, EquipIndex);
	}

	else
	{
		ServerUnEquipItem(ItemFilter, EquipIndex);
	}
}


void UEquipmentComponent::AddQuickItem(FItemTable Item, bool bShiftLeft)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(AddQuickEquipmentItem(Item, bShiftLeft))
		{
			SetRefreshEquipments(true);			
		}
	}

	else
	{
		ServerAddQuickItem(Item, bShiftLeft);
	}
}

void UEquipmentComponent::AddQuickItemAt(FItemTable Item, int32 QuickIndex)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(AddQuickEquipmentItemAt(Item, QuickIndex))
		{
			SetRefreshEquipments(true);			
		}
	}

	else
	{
		ServerAddQuickItemAt(Item, QuickIndex);
	}
}


void UEquipmentComponent::RemoveQuickItemAt(EItemFilter ItemFilter, int32 EquipIndex, bool bShiftLeft)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(RemoveQuickEquipmentItemAt(ItemFilter, EquipIndex, bShiftLeft))
		{
			SetRefreshEquipments(true);			
		}
	}

	else
	{
		ServerRemoveQuickItem(ItemFilter, EquipIndex, bShiftLeft);
	}
}

void UEquipmentComponent::ShiftLeftEquipments(EMouseWheel MouseWheel)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		bool bChanged = false;
		
		switch (MouseWheel)
		{
		case EMouseWheel::Wheel_Magic:
			break;

		case EMouseWheel::Wheel_Weapon:
			bChanged = ShilftLeftEquipmentSlot(EItemFilter::Filter_Weapon);
			break;

		case EMouseWheel::Wheel_Shield:
			bChanged = ShilftLeftEquipmentSlot(EItemFilter::Filter_Shield);
			break;

		case EMouseWheel::Wheel_Potion:
			bChanged = ShilftLeftEquipmentSlot(EItemFilter::Filter_Potion);
			break;

		default: ;
		}
		
		if(bChanged)
		{
			SetRefreshEquipments(true);
		}
	}

	else
	{
		ServerChangeQuickSlot(MouseWheel);
	}
}

////////////////////////////////////////////////////////////////////////////
////  서버 ::

void UEquipmentComponent::ServerUseQuickItem_Implementation()
{
	UseQuickItem();

	// MulticastRefreshClients(this);
}

void UEquipmentComponent::ServerEquipItem_Implementation(FItemTable Item)
{
	EquipItem(Item);

	// MulticastRefreshClients(this);
}


void UEquipmentComponent::ServerUnEquipItem_Implementation(EItemFilter ItemFilter, int32 EquipIndex)
{
	UnEquipItem(ItemFilter, EquipIndex);

	// MulticastRefreshClients(this);
}

void UEquipmentComponent::ServerAddQuickItem_Implementation(FItemTable Item, bool bShiftLeft)
{
	AddQuickItem(Item, bShiftLeft);

	// MulticastRefreshClients(this);
}

void UEquipmentComponent::ServerAddQuickItemAt_Implementation(FItemTable Item, int32 QuickIndex)
{
	AddQuickItemAt(Item, QuickIndex);

	// MulticastRefreshClients(this);
}

void UEquipmentComponent::ServerRemoveQuickItem_Implementation(EItemFilter ItemFilter, int32 QuickIndex, bool bShiftLeft)
{
	RemoveQuickItemAt(ItemFilter, QuickIndex, bShiftLeft);

	// MulticastRefreshClients(this);
}

void UEquipmentComponent::ServerChangeQuickSlot_Implementation(EMouseWheel MouseWheel)
{
	ShiftLeftEquipments(MouseWheel);

	// MulticastRefreshClients(this);
}

////////////////////////////////////////////////////////////////////////////
////

void UEquipmentComponent::MulticastRefreshClients_Implementation(UEquipmentComponent* EquipComponent)
{
	EquipComponent->SetRefreshEquipments(true);

	SetRefreshEquipments(true);
}

////////////////////////////////////////////////////////////////////////////
////

bool UEquipmentComponent::UseQuickInventoryItem()
{
	const int32 QuickIndex = 0;
	
	if(QuickPotions.Num() > 0)
	{
		const int32 Count = QuickPotions[QuickIndex].Count;
		
		if(Count > 0)
		{
			// 효과 적용
			
			QuickPotions[QuickIndex].Count--;
					
			return true;
		}
	}

	return false;
}

bool UEquipmentComponent::EquipInventoryItem(FItemTable Item)
{
	if(OwnerController)
	{
		// 착용 데미지 증가라던가 하자
		
		switch (Item.ItemFilter)
		{
			case EItemFilter::Filter_Weapon:
				SetCurrentWeapon(true);
				OwnerController->ClientUpdateQuickBar(1, Item.Icon, Item.Name);
				break;

			case EItemFilter::Filter_Shield:
				OwnerController->ClientUpdateQuickBar(2, Item.Icon, Item.Name);
				break;

			case EItemFilter::Filter_Potion:
				OwnerController->ClientUpdateQuickBar(3, Item.Icon, Item.Name);
				break;

			default: ;
		}
		
		return true;
	}
	
	return false;
}

bool UEquipmentComponent::UnEquipEquipmentItem(EItemFilter ItemFilter, int32 EquipIndex)
{
	if(OwnerController)
	{
		// 해제 버프 디버프 적용

		switch (ItemFilter)
		{
		case EItemFilter::Filter_Weapon:
			SetCurrentWeapon(false);
			OwnerController->ClientClearQuickBar(1);
			break;

		case EItemFilter::Filter_Shield:
			OwnerController->ClientClearQuickBar(2);
			break;

		case EItemFilter::Filter_Potion:
			OwnerController->ClientClearQuickBar(3);
			break;

		default: ;
		}

		RemoveQuickItemAt(ItemFilter, 0, false);
		
		return true;
	}
	
	return false;
}


bool UEquipmentComponent::AddQuickEquipmentItem(FItemTable Item, bool bShiftLeft)
{
	const EItemFilter ItemFilter = Item.ItemFilter;
	int32 ArmorIndex;
	
	switch (ItemFilter)
	{
	case EItemFilter::Filter_Weapon:
		AddItemInEquipment(QuickWeapons, Item, bShiftLeft);
		return true;
		
	case EItemFilter::Filter_Shield:
		AddItemInEquipment(QuickShields, Item, bShiftLeft);
		return true;
		
	case EItemFilter::Filter_Armor:
		ArmorIndex = static_cast<int32>(Item.ItemType) - 2;
		if(ArmorMap.Contains(ArmorIndex))
		{
			OwnerCharacter->AddItem(ArmorMap[ArmorIndex]);
		}
		ArmorMap.Add(ArmorIndex, Item);
		return true;
		
	case EItemFilter::Filter_Potion:
		AddItemInEquipment(QuickPotions, Item, bShiftLeft);
		return true;
		
	default:
		return false;
	}
}

void UEquipmentComponent::AddItemInEquipment(TArray<FItemTable>& QuickEquipments, FItemTable Item, bool bShiftLeft)
{
	QuickEquipments.Add(Item);

	if(!bShiftLeft)
	{
		EquipWeight += Item.Weight * Item.Count;
	}
	
	if(QuickEquipments.Num() == 1) // 처음 퀵등록을 한다면 바로 장착함.
	{
		EquipItem(Item);
	}

	else if(QuickEquipments.Num() > 4 && !bShiftLeft) // 퀵 아이템이 4개가 넘으면 1번째 아이템을 인벤토릴 돌려줌
	{
		OwnerCharacter->AddItem(QuickEquipments[1]);

		RemoveQuickItemAt(QuickEquipments[1].ItemFilter, 0, bShiftLeft);
	}
}

bool UEquipmentComponent::AddQuickEquipmentItemAt(FItemTable Item, int32 EquipIndex)
{
	const EItemFilter ItemFilter = Item.ItemFilter;
	
	switch (ItemFilter)
	{
	case EItemFilter::Filter_Weapon:
		AddItemInEquipmentAt(QuickWeapons, EquipIndex, Item);
		return true;
		
	case EItemFilter::Filter_Shield:
		
		return true;
				
	case EItemFilter::Filter_Potion:
		AddItemInEquipmentAt(QuickPotions, EquipIndex, Item);
		return true;
		
	default:
		return false;
	}
}

void UEquipmentComponent::AddItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, int32 EquipIndex, FItemTable Item)
{
	QuickEquipments.Insert(Item, EquipIndex);
	EquipWeight += Item.Weight * Item.Count;
	
	if(QuickEquipments.Num() == 1) // 처음 퀵등록을 한다면 바로 장착함.
	{
		EquipItem(Item);
	}

	else if(QuickEquipments.Num() > 4) // 퀵 아이템이 4개가 넘으면 1번째 아이템을 인벤토릴 돌려줌
	{
		OwnerCharacter->AddItem(QuickEquipments[1]);

		RemoveQuickItemAt(QuickEquipments[1].ItemFilter, 0, false);
	}
}


bool UEquipmentComponent::RemoveQuickEquipmentItemAt(EItemFilter ItemFilter, int32 EquipIndex, bool bShiftLeft)
{
	if(OwnerController)
	{
		switch (ItemFilter)
		{
		case EItemFilter::Filter_Weapon:
			RemoveItemInEquipmentAt(QuickWeapons, EquipIndex, bShiftLeft);
			return true;
		
		case EItemFilter::Filter_Shield:
		
			return true;
				
		case EItemFilter::Filter_Potion:
			RemoveItemInEquipmentAt(QuickPotions, EquipIndex, bShiftLeft);
			return true;

		default:
			return false;
		}
		
	}

	return false;
}

void UEquipmentComponent::RemoveItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, int32 EquipIndex, bool bShiftLeft)
{
	if(EquipIndex < QuickEquipments.Num())
	{
		if(!bShiftLeft)
		{
			EquipWeight -= QuickEquipments[EquipIndex].Weight * QuickEquipments[EquipIndex].Count;

			OwnerCharacter->AddItem(QuickEquipments[EquipIndex]);
		}
		
		QuickEquipments.RemoveAt(EquipIndex);

		if(EquipIndex == 0 && QuickEquipments.Num() > 0) 
		{
			EquipItem(QuickEquipments[0]);
		}
	}
}

bool UEquipmentComponent::ShilftLeftEquipmentSlot(EItemFilter ItemFilter)
{
	if(OwnerController)
	{
		switch (ItemFilter)
		{
		case EItemFilter::Filter_Weapon:
			ShiftLeft(QuickWeapons, ItemFilter);
			break;

		case EItemFilter::Filter_Shield:
			ShiftLeft(QuickShields, ItemFilter);
			break;
			
		case EItemFilter::Filter_Armor:
			break;
			
		case EItemFilter::Filter_Potion:
			ShiftLeft(QuickPotions, ItemFilter);
			break;
			
		default:
			return false;
		}

		SetRefreshEquipments(true);
		
		return true;
	}
	
	return false;
}

void UEquipmentComponent::ShiftLeft(TArray<FItemTable>& QuickEquipments, EItemFilter ItemFilter)
{
	if(QuickEquipments.Num() > 1)
	{
		AddQuickItem(QuickEquipments[0], true);
		RemoveQuickItemAt(ItemFilter, 0, true);
	}
}

TArray<UTexture2D*> UEquipmentComponent::GetItemIconsByQuickIndex(EItemFilter ItemFilter) const
{
	TArray<UTexture2D*> TempIcons;

	switch (ItemFilter)
	{
	case EItemFilter::Filter_Weapon:
		for(auto& Weapon  : QuickWeapons)
		{
			TempIcons.Add(Weapon.Icon);
		}
		break;

	case EItemFilter::Filter_Potion:
		for(auto& Weapon  : QuickPotions)
		{
			TempIcons.Add(Weapon.Icon);
		}
		break;

	default:
		break;
	}

	return TempIcons;
}

float UEquipmentComponent::GetWeaponDamage(EPlayerAttack PlayerAttack)
{
	if(QuickWeapons.Num() > 0)
	{
		switch (PlayerAttack)
		{
		case EPlayerAttack::Player_LightAttack: 
			return QuickWeapons[0].LightDamage;

		case EPlayerAttack::Player_HeavyAttack:
			return QuickWeapons[0].HeavyDamage;
			
		default:
			return 0.f;
		}
	}

	return 0.f;
}

TArray<FText> UEquipmentComponent::GetDamageText(EItemFilter ItemFilter)
{
	TArray<FText> Damages;
	
	if(ItemFilter == EItemFilter::Filter_Weapon)
	{
		for(auto& Weapon : QuickWeapons)
		{
			Damages.Add(FText::FromString(FString::SanitizeFloat(Weapon.LightDamage)));
		}
	}

	else
	{
		for(auto& Shield : QuickShields)
		{
			Damages.Add(FText::FromString(FString::SanitizeFloat(Shield.LightDamage)));
		}
	}

	return Damages;
}

FText UEquipmentComponent::GetArmorText(bool bMeleeArmor)
{
	float TotalArmor = 0.f;
	
	for(auto& Armor : ArmorMap)
	{
		if(bMeleeArmor)
		{
			TotalArmor += Armor.Value.MeleeArmor;
		}

		else
		{
			TotalArmor += Armor.Value.MagicArmor;
		}
	}

	return FText::FromString(FString::SanitizeFloat(TotalArmor));
}

void UEquipmentComponent::HoverEquipmentSlot(EItemFilter ItemFilter, int32 EquipIndex)
{
	if(OwnerController)
	{
		switch (ItemFilter)
		{
		case EItemFilter::Filter_Weapon:
			UpdateItemDescription(QuickWeapons, EquipIndex);
			break;

		case EItemFilter::Filter_Shield:
			UpdateItemDescription(QuickShields, EquipIndex);
			break;

		case EItemFilter::Filter_Armor:
			if(ArmorMap.Contains(EquipIndex))
			{
				OwnerController->ClientUpdateItemDescription(ArmorMap[EquipIndex]);
			}
			break;
			

		case EItemFilter::Filter_Potion:
			UpdateItemDescription(QuickPotions, EquipIndex);
			break;

		default: ;
		}
	}
}

void UEquipmentComponent::UpdateItemDescription(TArray<FItemTable>& QuickEquipments, int32 EquipIndex)
{
	if(EquipIndex < QuickEquipments.Num())
	{
		OwnerController->ClientUpdateItemDescription(QuickEquipments[EquipIndex]);
	}
}


void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, OwnerCharacter);
	DOREPLIFETIME(UEquipmentComponent, OwnerController);
	DOREPLIFETIME(UEquipmentComponent, ArmorMap);
	DOREPLIFETIME(UEquipmentComponent, QuickWeapons);
	DOREPLIFETIME(UEquipmentComponent, QuickShields);
	DOREPLIFETIME(UEquipmentComponent, QuickPotions);
	DOREPLIFETIME(UEquipmentComponent, EquipWeight);
}
