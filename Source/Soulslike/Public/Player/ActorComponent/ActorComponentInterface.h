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

	////////////////////////////////////////////////////////////////////////////
	//// 인벤토리	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
	class UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    TArray<FItemTable> GetInventory();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryComponent")
    void AddItem(FItemTable Item);

	////////////////////////////////////////////////////////////////////////////
	//// 장비
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	class UEquipmentComponent* GetEquipmentComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
	float GetEquipWeight();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipmentComponent")
    void AddQuickItemAt(FItemTable Item, int32 EquipIndex);
	
};
