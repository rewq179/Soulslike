// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "ActorComponentInterface.generated.h"

class UInventoryComponent;
class UStatComponent;
class AInteractDoor;
class APickUpActor;

// This class does not need to be modified.
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
	////////////////////////////////////////////////////////////////////////////
	//// 상호작용 컴포넌트

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void SetInteractDoor(AInteractDoor* Door);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
    void SetPickUpActor(APickUpActor* Actor);

	////////////////////////////////////////////////////////////////////////////
	//// 스탯 컴포넌트

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	UStatComponent* GetStatComponent();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	int32 GetSoulsCount();

	////////////////////////////////////////////////////////////////////////////
	//// 인벤토리 컴포넌트
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void UseItem(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void UnEquipItem(int32 EquipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SwapItem(int32 FromIndex, int32 ToIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void MoveItem(int32 FromIndex, int32 ToIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void RemoveItemAt(int32 SlotIndex, int32 Count);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Component")
	void LockItemAt(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Component")
	void SetQuickItemAt(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Component")
	void SortItem();
};
