// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataType.h"
#include "ActorComponentInterface.generated.h"

UINTERFACE(MinimalAPI)
class UActorComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 용도: SoulCharacter의 ActorComponent 함수를 SoulCharacter에서 사용하기 위함
 */

class SOULSLIKE_API IActorComponentInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractComponent")
    void SetInteractDoor(class AInteractDoor* Door);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractComponent")
    void SetPickUpActor(class APickUpActor* Actor);

	////////////////////////////////////////////////////////////////////////////
	//// 스탯
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatComponent")
	class UStatComponent* GetStatComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StatComponent")
	int32 GetSoulsCount();

	////////////////////////////////////////////////////////////////////////////
	//// 인벤토리	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
	class UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    TArray<FItemTable> GetInventory();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    FItemTable GetInventoryItemAt(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void AddItem(FItemTable Item);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void UseItem(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void SwapItem(int32 FromIndex, int32 ToIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void MoveItem(int32 FromIndex, int32 ToIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void RemoveItemAt(int32 SlotIndex, int32 Count);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void LockItemAt(int32 SlotIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void SortItem();

	////////////////////////////////////////////////////////////////////////////
	//// 장비
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	class UEquipmentComponent* GetEquipmentComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	float GetEquipWeight();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	void UnEquipItem(EItemFilter ItemFilter, int32 EquipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	void AddQuickItem(FItemTable Item);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
    void AddQuickItemAt(FItemTable Item, int32 EquipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	void RemoveQuickItemAt(EItemFilter ItemFilter, int32 EquipIndex);
};
