// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "InventoryComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;
class AWeapon;

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
	UPROPERTY(ReplicatedUsing = OnRep_RefreshInventory, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	bool bRefreshInventory;
	
	UFUNCTION()
	void OnRep_RefreshInventory();

public:
	void SetRefreshInventory(bool bRefresh);

	////////////////////////////////////////////////////////////////////////////
	////
	
    void UseItem(int32 SlotIndex);

	void AddItem(FItemTable Item);
	/** 새로운 Index에 아이템을 넣을 때*/
	void AddItemAt(FItemTable Item, int32 SlotIndex);

	void RemoveItem(FItemTable Item);
	/** 해당 Index의 아이템을 제거할 때 */
	void RemoveItemAt(int32 SlotIndex, int32 Count);

	void SwapItem(int32 FromIndex, int32 ToIndex);
	
	/** FromIndex의 아이템을 ToIndex로 옮긴다 */
    void MoveItem(int32 FromIndex, int32 ToIndex);

    void LockItemAt(int32 SlotIndex);

	void SortItem();

	bool IsEnoughCount(FItemTable Item);

protected:
	UFUNCTION(Server, Reliable)
	void ServerUseItem(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerAddItem(FItemTable Item);

	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(FItemTable Item);

	UFUNCTION(Server, Reliable)
    void ServerAddItemAt(FItemTable Item, int32 SlotIndex);

	UFUNCTION(Server, Reliable)
    void ServerRemoveItemAt(int32 SlotIndex, int32 Count);

	UFUNCTION(Server, Reliable)
    void ServerSwapItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerMoveItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerLockItemAt(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
    void ServerSortItem();
		
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRefreshClients(UInventoryComponent* InvComponent);

public:
    bool UseInventoryItem(int32 SlotIndex);
	
	void UsePotion(int32 SlotIndex);
	
	/** True : 아이템 1개를 인벤토리에 Add */
	bool AddInventoryItem(FItemTable Item);
	bool AddInventoryItemAt(FItemTable Item, int32 SlotIndex);

	/** True : 아이템 1개를 제거한다면 */
	bool RemoveInventoryItem(FItemTable Item);
	bool RemoveInventoryItemAt(int32 SlotIndex, int32 Count);

	/** True : 이동에 성공하면 */
	bool MoveInventoryItem(int32 FromIndex, int32 ToIndex);

	bool LockInventoryItemAt(int32 SlotIndex);

	bool SortInventoryItem();
	
	int32 GetInventoryIndex(int32 Id, int32 StartIndex);

public:
	UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FItemTable> GetInventory() const {return Inventory;}

    FORCEINLINE FItemTable GetInventoryItem(int32 SlotIndex) const {return Inventory[SlotIndex];}

	float GetFreeWeight() const;
	
	UFUNCTION(BlueprintCallable)
	float GetCurWeight() const;
	UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetMaxWeight() const {return MaxWeight;}
    
    UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurSlot() const {return MaxSlot - FreeSlot;}
	UFUNCTION(BlueprintCallable)
    FORCEINLINE int32 GetMaxSlot() const {return MaxSlot;}
    UFUNCTION(BlueprintCallable)
	FText GetWeightText();

	UFUNCTION(BlueprintCallable)
	void HoverInventorySlot(int32 SlotIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
