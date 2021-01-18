// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SoulPlayerController.h"
#include "Player/SoulCharacter.h"

#include "Net/UnrealNetwork.h"

ASoulPlayerController::ASoulPlayerController()
{

}

////////////////////////////////////////////////////////////////////////////
//// 인터페이스

void ASoulPlayerController::ShowEquipmentWidget_Implementation()
{
	OnShowShowMenuHUD(false);

	bPlayerHUDActive = true;
	
	ClientShowEquipment();
}

void ASoulPlayerController::ShowInventoryWidget_Implementation()
{
	OnShowShowMenuHUD(false);

	bPlayerHUDActive = true;
	
	ClientShowInventory();
}

void ASoulPlayerController::ShowStatusWidget_Implementation()
{
	OnShowShowMenuHUD(false);

	bPlayerHUDActive = true;
	
	ClientShowStatus();
}

void ASoulPlayerController::ShowOptionWidget_Implementation()
{
	OnShowShowMenuHUD(false);

	bPlayerHUDActive = true;
	
	ClientShowOption();
}

void ASoulPlayerController::ClearItemDescription_Implementation()
{
	ClientClearItemDescription();
}

////////////////////////////////////////////////////////////////////////////
////

void ASoulPlayerController::ClientTurnOffHUD_Implementation()
{
	bPlayerHUDActive = false;
	
	OnTurnOffHUD();
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
//// 메뉴

void ASoulPlayerController::ClientShowMenuHUD_Implementation()
{
	OnShowShowMenuHUD(true);
}

void ASoulPlayerController::ClientBackMenuHUD_Implementation()
{
	if(bPlayerHUDActive)
	{
		OnTurnOffHUD();

		bPlayerHUDActive = false;

		OnShowShowMenuHUD(true);
	}
}


////////////////////////////////////////////////////////////////////////////
//// 인벤토리

void ASoulPlayerController::ClientShowInventory_Implementation()
{
	OnShowInventory(true);
}

void ASoulPlayerController::ClientUpdateInventory_Implementation()
{
	OnUpdateInventory();	
}

////////////////////////////////////////////////////////////////////////////
//// 장비

void ASoulPlayerController::ClientShowEquipment_Implementation()
{
	OnShowEquipment(true);
}

void ASoulPlayerController::ClientUpdateEquipmentSlot_Implementation()
{
	OnUpdateEquipmentSlot();
}

void ASoulPlayerController::ClientUpdateQuickBar_Implementation(int32 QuickIndex, UTexture2D* Icon, const FText& Name)
{
	OnUpdateQuickBar(QuickIndex, Icon, Name);
}

void ASoulPlayerController::ClientClearQuickBar_Implementation(int32 QuickIndex)
{
	OnClearQuickBar(QuickIndex);
}

////////////////////////////////////////////////////////////////////////////
//// 아이템 설명 HUD

void ASoulPlayerController::ClientUpdateItemDescription_Implementation(FItemTable ItemTable)
{
	OnUpdateItemDescription(ItemTable);
}

void ASoulPlayerController::ClientClearItemDescription_Implementation()
{
	OnClearItemDescription();
}

////////////////////////////////////////////////////////////////////////////
//// 스테이터스

void ASoulPlayerController::ClientShowStatus_Implementation()
{
	OnShowStatus(true);	
}

////////////////////////////////////////////////////////////////////////////
//// 옵션

void ASoulPlayerController::ClientShowOption_Implementation()
{
	OnShowOption(true);
}

////////////////////////////////////////////////////////////////////////////
//// 기타

void ASoulPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
