// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/EquipmentComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "Player/Weapon.h"

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
//// 초기화

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

////////////////////////////////////////////////////////////////////////////
//// 장비 슬롯 및 현재 무기 업데이트

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
//// 외부에서 실행되는 함수

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
			bChanged = ShiftLeftEquipmentSlot(EItemFilter::Filter_Weapon);
			break;

		case EMouseWheel::Wheel_Shield:
			bChanged = ShiftLeftEquipmentSlot(EItemFilter::Filter_Shield);
			break;

		case EMouseWheel::Wheel_Potion:
			bChanged = ShiftLeftEquipmentSlot(EItemFilter::Filter_Potion);
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
//// 클라이언트가 서버에게 요청하는 함수

void UEquipmentComponent::ServerUseQuickItem_Implementation()
{
	UseQuickItem();
}

void UEquipmentComponent::ServerEquipItem_Implementation(FItemTable Item)
{
	EquipItem(Item);
}


void UEquipmentComponent::ServerUnEquipItem_Implementation(EItemFilter ItemFilter, int32 EquipIndex)
{
	UnEquipItem(ItemFilter, EquipIndex);
}

void UEquipmentComponent::ServerAddQuickItem_Implementation(FItemTable Item, bool bShiftLeft)
{
	AddQuickItem(Item, bShiftLeft);
}

void UEquipmentComponent::ServerAddQuickItemAt_Implementation(FItemTable Item, int32 QuickIndex)
{
	AddQuickItemAt(Item, QuickIndex);
}

void UEquipmentComponent::ServerRemoveQuickItem_Implementation(EItemFilter ItemFilter, int32 QuickIndex, bool bShiftLeft)
{
	RemoveQuickItemAt(ItemFilter, QuickIndex, bShiftLeft);
}

void UEquipmentComponent::ServerChangeQuickSlot_Implementation(EMouseWheel MouseWheel)
{
	ShiftLeftEquipments(MouseWheel);
}

////////////////////////////////////////////////////////////////////////////
//// 멀티캐스트

void UEquipmentComponent::MulticastRefreshClients_Implementation(UEquipmentComponent* EquipComponent)
{
	EquipComponent->SetRefreshEquipments(true);

	SetRefreshEquipments(true);
}

////////////////////////////////////////////////////////////////////////////
//// 실제로 동작되는 함수

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


bool UEquipmentComponent::RemoveQuickEquipmentItemAt(const EItemFilter ItemFilter, const int32 EquipIndex, const bool bShiftLeft)
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

void UEquipmentComponent::RemoveItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, const int32 EquipIndex, const bool bShiftLeft)
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

bool UEquipmentComponent::ShiftLeftEquipmentSlot(const EItemFilter ItemFilter)
{
	if(OwnerController)
	{
		switch (ItemFilter)
		{
		case EItemFilter::Filter_Weapon:
			ShiftLeftArrayByFilter(QuickWeapons, ItemFilter);
			break;

		case EItemFilter::Filter_Shield:
			ShiftLeftArrayByFilter(QuickShields, ItemFilter);
			break;
			
		case EItemFilter::Filter_Armor:
			break;
			
		case EItemFilter::Filter_Potion:
			ShiftLeftArrayByFilter(QuickPotions, ItemFilter);
			break;
			
		default:
			return false;
		}

		SetRefreshEquipments(true);
		
		return true;
	}
	
	return false;
}

void UEquipmentComponent::ShiftLeftArrayByFilter(TArray<FItemTable>& QuickEquipments, const EItemFilter ItemFilter)
{
	if(QuickEquipments.Num() > 1) 
	{
		AddQuickItem(QuickEquipments[0], true);

		RemoveQuickItemAt(ItemFilter, 0, true);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 게터

float UEquipmentComponent::GetWeaponDamage(const EPlayerAttack PlayerAttack)
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

FText UEquipmentComponent::GetArmorText(const bool bMeleeArmor)
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

TArray<FText> UEquipmentComponent::GetDamageTexts(const EItemFilter ItemFilter)
{
	TArray<FText> Damages;
	
	if(ItemFilter == EItemFilter::Filter_Weapon)
	{
		for(int32 EquipIndex = 0; EquipIndex < 4; ++EquipIndex)
		{
			if(EquipIndex < QuickWeapons.Num())
			{
				Damages.Add(FText::FromString(FString::SanitizeFloat(QuickWeapons[EquipIndex].LightDamage)));
				
				continue;
			}
			
			Damages.Add(FText::FromString("0"));
		}
	}

	else
	{
		for(int32 EquipIndex = 0; EquipIndex < 4; ++EquipIndex)
		{
			if(EquipIndex < QuickShields.Num())
			{
				Damages.Add(FText::FromString(FString::SanitizeFloat(QuickShields[EquipIndex].LightDamage)));
				
				continue;
			}
			
			Damages.Add(FText::FromString("0"));
		}
	}

	return Damages;
}

TArray<UTexture2D*> UEquipmentComponent::GetItemIconsByQuickIndex(const EItemFilter ItemFilter) const
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

////////////////////////////////////////////////////////////////////////////
//// HUD

void UEquipmentComponent::HoverEquipmentSlot(const EItemFilter ItemFilter, const int32 EquipIndex)
{
	if(OwnerController == nullptr)
	{
		return;
	}
	
	switch (ItemFilter)
	{
	case EItemFilter::Filter_Weapon:
		UpdateItemDescriptionByFilter(QuickWeapons, EquipIndex);
		break;

	case EItemFilter::Filter_Shield:
		UpdateItemDescriptionByFilter(QuickShields, EquipIndex);
		break;

	case EItemFilter::Filter_Armor:
		if(ArmorMap.Contains(EquipIndex))
		{
			OwnerController->ClientUpdateItemDescription(ArmorMap[EquipIndex]);
		}
		break;
			

	case EItemFilter::Filter_Potion:
		UpdateItemDescriptionByFilter(QuickPotions, EquipIndex);
		break;

	default: ;
	}
}

void UEquipmentComponent::UpdateItemDescriptionByFilter(TArray<FItemTable>& QuickEquipments, const int32 EquipIndex) const
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
