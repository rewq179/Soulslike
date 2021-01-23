// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "InventoryComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;
class AWeapon;

/**
* 용도: 플레이어의 인벤토리
*
* 인벤토리에서는 아이엠을 추가, 제거, 스왑, 이동, 퀵슬롯 등록이 가능하다.
* 인벤토리에는 최대 무게와 슬롯이 존재한다.
* 
* 아이템 추가는 1개씩 For문을 통해 넣어진다
* 아이템 제거는 1~Max까지 가능하다.
*/

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
	//// 외부에서 실행되는 함수

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
    void UseItem(int32 SlotIndex);

	void AddItem(FItemTable Item);
	void AddItemAt(FItemTable Item, int32 SlotIndex);

	void RemoveItem(FItemTable Item);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveItemAt(int32 SlotIndex, int32 Count);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void SortItem();
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void SwapItem(int32 FromIndex, int32 ToIndex);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void MoveItem(int32 FromIndex, int32 ToIndex);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void LockItemAt(int32 SlotIndex);

	bool IsEnoughCount(FItemTable Item);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 클라이언트가 서버에게 요청하는 함수
	
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
    void ServerSortItem();
	
	UFUNCTION(Server, Reliable)
    void ServerSwapItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerMoveItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerLockItemAt(int32 SlotIndex);
		
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRefreshClients(UInventoryComponent* InvComponent);

public:
	////////////////////////////////////////////////////////////////////////////
	//// 멀티 캐스트
	
    bool UseInventoryItem(int32 SlotIndex);

	////////////////////////////////////////////////////////////////////////////
	//// 실제로 동작되는 함수
	
	void UsePotion(int32 SlotIndex);
	
	bool AddInventoryItem(FItemTable Item);
	bool AddInventoryItemAt(FItemTable Item, int32 SlotIndex);

	bool RemoveInventoryItem(FItemTable Item);
	bool RemoveInventoryItemAt(int32 SlotIndex, int32 Count);

	bool SortInventoryItem();
	bool MoveInventoryItem(int32 FromIndex, int32 ToIndex);
	bool LockInventoryItemAt(int32 SlotIndex);

	void AddWeight(float Weight);

	////////////////////////////////////////////////////////////////////////////
	//// 게터
	
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FORCEINLINE FItemTable GetInventoryItemAt(const int32 SlotIndex) {return Inventory[SlotIndex];}
	FORCEINLINE	bool IsEnoughWeight(const float Weight) const {return Weight + GetCurWeight() < MaxWeight; }
	
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
    FORCEINLINE TArray<FItemTable> GetInventory() const {return Inventory;}

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FORCEINLINE float GetMaxWeight() const {return MaxWeight;}
    
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FORCEINLINE int32 GetCurSlot() const {return MaxSlot - FreeSlot;}

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FORCEINLINE int32 GetMaxSlot() const {return MaxSlot;}

	/** 해당 Id와 같은 아이템의 인벤토리내 위치(Index)를 얻을 수 있다. */
	int32 GetInventoryIndex(int32 Id, int32 StartIndex);

	/** 현재 보유한 무게(10.5, 20.3, ...)로 출력한다. */
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	float GetCurWeight() const;

	/** Weight : {Cur}/{Max} 형태로 출력한다 */
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FText GetTotalWeightText() const;
	
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void HoverInventorySlot(const int32 SlotIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
