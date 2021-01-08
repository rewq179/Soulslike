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

	/** 0:무기, 1:방패, 2:갑옷, 3:헬멧 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<int32, FItemTable> EquipmentMap;
	
	/** 0:소울, 1:무기, 2:방패, 3:포션 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<int32, int32> QuickSlotMap;

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

	////////////////////////////////////////////////////////////////////////////
	//// 장착 아이템
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Weapon, BlueprintReadOnly, Category = Equipment)
	bool bWeaponEquiped;

	UFUNCTION()
    void OnRep_Weapon();

public:
    void SetCurrentWeapon(bool bEquip);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Weapon, BlueprintReadOnly, Category = Equipment)
	bool bShieldEquiped;

	UFUNCTION()
    void OnRep_Shield();

public:
    void SetCurrentShield(bool bEquip);


	////////////////////////////////////////////////////////////////////////////
	////
	///
    void UseItem(int32 SlotIndex);

	void UseQuickItem(int32 QuickIndex);

	void UnEquipItem(int32 EquipIndex);
	
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

    void SetQuickItemAt(int32 SlotIndex);

	void SortItem();

	bool IsEnoughCount(FItemTable Item);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseQuickItem(int32 QuickIndex);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerUnEquipItem(int32 EquipIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(FItemTable Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(FItemTable Item);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddItemAt(FItemTable Item, int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerRemoveItemAt(int32 SlotIndex, int32 Count);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSwapItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveItem(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLockItemAt(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetQuickItemAt(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSortItem();
		
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRefreshClients(UInventoryComponent* InvComponent);

public:
    bool UseInventoryItem(int32 SlotIndex);
	bool EquipInventoryItem(int32 SlotIndex);
	bool UnEquipEquipmentItem(int32 EquipIndex);
	void UsePotion(int32 SlotIndex);
	bool UseQuickInventoryItem(int32 QuickIndex);
	
	/** True : 아이템 1개를 인벤토리에 Add */
	bool AddInventoryItem(FItemTable Item);
	bool AddInventoryItemAt(FItemTable Item, int32 SlotIndex);

	/** True : 아이템 1개를 제거한다면 */
	bool RemoveInventoryItem(FItemTable Item);
	bool RemoveInventoryItemAt(int32 SlotIndex, int32 Count);

	/** True : 이동에 성공하면 */
	bool MoveInventoryItem(int32 FromIndex, int32 ToIndex);

	bool LockInventoryItemAt(int32 SlotIndex);

	bool SetQuickInventoryItemAt(int32 SlotIndex);

	bool SortInventoryItem();
	
	int32 GetInventoryIndex(int32 Id, int32 StartIndex);

	UFUNCTION(BlueprintCallable)
	void GetInventoryItemsByFilter(TArray<FItemTable>& TempInventory, TArray<int32>& TempInvIndex, EItemFilter Filter);

	UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FItemTable> GetInventory() const {return Inventory;}

	UFUNCTION(BlueprintCallable)
    FORCEINLINE TMap<int32, FItemTable> GetEquipment() const {return EquipmentMap;}
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurWeight() const {return MaxWeight - FreeWeight;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurSlot() const {return MaxSlot - FreeSlot;}

	UFUNCTION(BlueprintCallable)
    FORCEINLINE int32 GetMaxSlot() const {return MaxSlot;}

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
