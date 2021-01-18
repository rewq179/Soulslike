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
 * 
 */
class SOULSLIKE_API IActorComponentInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
    void SetInteractDoor(class AInteractDoor* Door);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
    void SetPickUpActor(class APickUpActor* Actor);

	////////////////////////////////////////////////////////////////////////////
	//// 스탯
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	class UStatComponent* GetStatComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	int32 GetSoulsCount();

	////////////////////////////////////////////////////////////////////////////
	//// 인벤토리	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	class UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    TArray<FItemTable> GetInventory();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    FItemTable GetInventoryItemAt(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void AddItem(FItemTable Item);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void UseItem(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void SwapItem(int32 FromIndex, int32 ToIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void MoveItem(int32 FromIndex, int32 ToIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void RemoveItemAt(int32 SlotIndex, int32 Count);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void LockItemAt(int32 SlotIndex);
 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void SortItem();

	////////////////////////////////////////////////////////////////////////////
	//// 장비
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	class UEquipmentComponent* GetEquipmentComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	float GetEquipWeight();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void UnEquipItem(EItemFilter ItemFilter, int32 EquipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void AddQuickItem(FItemTable Item);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
    void AddQuickItemAt(FItemTable Item, int32 EquipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void RemoveQuickItemAt(EItemFilter ItemFilter, int32 EquipIndex);
};
