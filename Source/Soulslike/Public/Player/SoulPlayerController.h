// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API ASoulPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASoulPlayerController();

	/** True : 인벤토리 HUD On, False : Off */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	bool bInventoryActive;

	/** True : 장비 HUD On, False : Off */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	bool bEquipmentActive;
	
	////////////////////////////////////////////////////////////////////////////
	//// 루팅
	
	UFUNCTION(Client, Reliable)
	void ClientShowPickUpName(const FText& PickUpName);

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
	void OnShowPickUpName(const FText& PickUpName);

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateHpBar(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void OnUpdateHpValue(float CurHp, float MaxHp);

	UFUNCTION(Client, Reliable)
	void ClientUpdateStaminaBar(float CurStamina, float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void OnUpdateStaminaValue(float CurStamina, float MaxStamina);

	UFUNCTION(Client, Reliable)
    void ClientUpdateSoulsCount(int32 SoulsCount);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
    void OnUpdateSoulsCount(int32 SoulsCount);

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 사망
	
	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void ShowDeadScreenWidget();

	////////////////////////////////////////////////////////////////////////////
	//// 적 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientShowEnemyHpBar(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnShowEnemyHpBar(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossName(const FText& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnUpdateBossName(const FText& Name);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossHp(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnUpdateBossHp(float CurHp, float MaxHp);

	////////////////////////////////////////////////////////////////////////////
	//// 상호작용
	
	UFUNCTION(Client, Reliable)
	void ClientShowInteractMessage(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
	void OnShowInteractMessage(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
	void OnUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	////////////////////////////////////////////////////////////////////////////
             	//// 인벤토리
	
	UFUNCTION(Client, Reliable)
	void ClientShowInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void OnShowInventory(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientUpdateInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
    void OnUpdateInventory();

	UFUNCTION(Client, Reliable)
    void ClientUpdateQuickSlot(int32 QuickIndex, UTexture2D* Icon, const FText& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
    void OnUpdateQuickSlot(int32 QuickIndex, UTexture2D* Icon, const FText& Name);

	UFUNCTION(Client, Reliable)
    void ClientClearQuickSlot(int32 QuickIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
    void OnClearQuickSlot(int32 QuickIndex);

	////////////////////////////////////////////////////////////////////////////
	//// 장비
	
	UFUNCTION(Client, Reliable)
	void ClientShowEquipment();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnShowEquipment(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientUpdateEquipmentSlot(int32 EquipIndex, UTexture2D* Icon);

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnUpdateEquipmentSlot(int32 EquipIndex, UTexture2D* Icon);

	UFUNCTION(Client, Reliable)
    void ClientClearEquipmentSlot(int32 EquipIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnClearEquipmentSlot(int32 EquipIndex);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
