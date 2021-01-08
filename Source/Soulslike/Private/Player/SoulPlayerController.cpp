// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SoulPlayerController.h"
#include "Player/SoulCharacter.h"

#include "Net/UnrealNetwork.h"

ASoulPlayerController::ASoulPlayerController()
{

}

////////////////////////////////////////////////////////////////////////////
//// 루팅

void ASoulPlayerController::ClientShowPickUpName_Implementation(const FText& PickUpName)
{
	OnShowPickUpName(PickUpName);
}

////////////////////////////////////////////////////////////////////////////
//// 플레이어 체력바

void ASoulPlayerController::ClientUpdateHpBar_Implementation(float CurHp, float MaxHp)
{
	OnUpdateHpValue(CurHp, MaxHp);
}

void ASoulPlayerController::ClientUpdateStaminaBar_Implementation(float CurStamina, float MaxStamina)
{
	OnUpdateStaminaValue(CurStamina, MaxStamina);
}


void ASoulPlayerController::ClientUpdateSoulsCount_Implementation(int32 SoulsCount)
{
	OnUpdateSoulsCount(SoulsCount);
}

////////////////////////////////////////////////////////////////////////////
//// 적 체력바 

void ASoulPlayerController::ClientShowEnemyHpBar_Implementation(bool bActive)
{
	OnShowEnemyHpBar(bActive);
}

void ASoulPlayerController::ClientUpdateBossName_Implementation(const FText& Name)
{
	OnUpdateBossName(Name);
}

void ASoulPlayerController::ClientUpdateBossHp_Implementation(float CurHp, float MaxHp)
{
	OnUpdateBossHp(CurHp, MaxHp);
}

////////////////////////////////////////////////////////////////////////////
//// 상호작용

void ASoulPlayerController::ClientShowInteractMessage_Implementation(bool bActive)
{
	OnShowInteractMessage(bActive);
}

void ASoulPlayerController::ClientUpdateInteractMessage_Implementation(const FText& Name, const FVector& ActorLocation)
{
	OnShowInteractMessage(true);
	OnUpdateInteractMessage(Name, ActorLocation);
}

////////////////////////////////////////////////////////////////////////////
//// 인벤토리

void ASoulPlayerController::ClientShowInventory_Implementation()
{
	bInventoryActive = !bInventoryActive;
	
	OnShowInventory(bInventoryActive);
}

void ASoulPlayerController::ClientUpdateInventory_Implementation()
{
	OnUpdateInventory();	
}

void ASoulPlayerController::ClientUpdateQuickSlot_Implementation(int32 QuickIndex, UTexture2D* Icon, const FText& Name)
{
	OnUpdateQuickSlot(QuickIndex, Icon, Name);
}

void ASoulPlayerController::ClientClearQuickSlot_Implementation(int32 QuickIndex)
{
	OnClearQuickSlot(QuickIndex);	
}

////////////////////////////////////////////////////////////////////////////
//// 장비

void ASoulPlayerController::ClientShowEquipment_Implementation()
{
	bEquipmentActive = !bEquipmentActive;

	OnShowEquipment(bEquipmentActive);
}

void ASoulPlayerController::ClientUpdateEquipmentSlot_Implementation(int32 QuickIndex, UTexture2D* Icon)
{
	OnUpdateEquipmentSlot(QuickIndex, Icon);
}

void ASoulPlayerController::ClientClearEquipmentSlot_Implementation(int32 EquipIndex)
{
	OnClearEquipmentSlot(EquipIndex);
}

////////////////////////////////////////////////////////////////////////////
//// 기타

void ASoulPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
