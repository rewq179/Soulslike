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

	////////////////////////////////////////////////////////////////////////////
	//// 루팅
	
	UFUNCTION(Client, Reliable)
	void ClientShowPickUpName(const FText& PickUpName);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnShowPickUpName(const FText& PickUpName);

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateHpBar(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnUpdateHpValue(float CurHp, float MaxHp);

	UFUNCTION(Client, Reliable)
	void ClientUpdateStaminaBar(float CurStamina, float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateStaminaValue(float CurStamina, float MaxStamina);

	UFUNCTION(Client, Reliable)
    void ClientUpdateSoulsCount(int32 SoulsCount);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
    void OnUpdateSoulsCount(int32 SoulsCount);

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 사망
	
	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void ShowDeadScreenWidget();

	////////////////////////////////////////////////////////////////////////////
	//// 적 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientShowEnemyHpBar(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnShowEnemyHpBar(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossName(const FText& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnUpdateBossName(const FText& Name);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossHp(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnUpdateBossHp(float CurHp, float MaxHp);

	////////////////////////////////////////////////////////////////////////////
	//// 상호작용
	
	UFUNCTION(Client, Reliable)
	void ClientShowInteractMessage(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnShowInteractMessage(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
	void OnUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	////////////////////////////////////////////////////////////////////////////
	//// 인벤토리
	
	UFUNCTION(Client, Reliable)
	void ClientShowInventory(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Widget)
	void OnShowInventory(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientUpdateInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Widget)
    void OnUpdateInventory();

	

	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
