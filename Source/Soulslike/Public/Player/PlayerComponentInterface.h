// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerComponentInterface.generated.h"

class UInventoryComponent;
class UStatComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULSLIKE_API IPlayerComponentInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	UStatComponent* GetStatComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	int32 GetSoulsCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void UseItem(int32 SlotIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void UnEquipItem(int32 EQuipIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SwapItem(int32 FromIndex, int32 ToIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void MoveItem(int32 FromIndex, int32 ToIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void RemoveItemAt(int32 SlotIndex, int32 Count);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void LockItemAt(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SetQuickItemAt(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SortItem();
};
