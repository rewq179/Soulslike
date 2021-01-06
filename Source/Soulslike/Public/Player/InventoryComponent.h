// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "InventoryComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FItemTable> Inventory;

	/** 현재 비어있는 슬롯 수 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 FreeSlot;

	/** 최대 보유 가능한 슬롯 수 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 MaxSlot;

	/** 현재 비어있는 무게 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	float FreeWeight;

	/** 최대 보유 가능한 무게 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	float MaxWeight;

	/** True : 인벤토리 새로고침 */
	UPROPERTY(ReplicatedUsing = OnRep_RefreshSlot, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	bool bRefreshSlot;

	/** RefNotify */
	UFUNCTION()
	void OnRep_RefreshSlot();

public:
	void SetRefreshSlot(bool bRefresh);

	void AddItem(FItemTable Item);
	/** 새로운 Index에 아이템을 넣을 때*/
	void AddItemAt(FItemTable Item, int32 SlotIndex);

	void RemoveItem(FItemTable Item);
	/** 해당 Index의 아이템을 제거할 때 */
	void RemoveItemAt(int32 SlotIndex, int32 Count);

	bool IsEnoughCount(FItemTable Item);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(FItemTable Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(FItemTable Item);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddItemAt(FItemTable Item, int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerRemoveItemAt(int32 SlotIndex, int32 Count);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRefreshClients(UInventoryComponent* InvComponent);

public:
	/** True : 아이템 1개를 인벤토리에 Add */
	bool AddInventoryItem(FItemTable Item);
	bool AddInventoryItemAt(FItemTable Item, int32 SlotIndex);

	/** True : 아이템 1개를 제거한다면 */
	bool RemoveInventoryItem(FItemTable Item);
	bool RemoveInventoryItemAt(int32 SlotIndex, int32 Count);
	
	int32 GetInventoryIndex(int32 Id, int32 StartIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
