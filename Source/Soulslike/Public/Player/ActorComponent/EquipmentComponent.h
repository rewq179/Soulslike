// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"


#include "EquipmentComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;
class AWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AWeapon> WeaponClass;

	/** 0:헬멧, 1:갑옷, 2:장갑, 3:신발 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<int32, FItemTable> ArmorMap;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FItemTable> QuickWeapons;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FItemTable> QuickShields;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FItemTable> QuickPotions;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	float EquipWeight;

protected:
	/** True : 장비 퀵슬롯 새로고침 */
	UPROPERTY(ReplicatedUsing = OnRep_RefreshEquipments, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bRefeshEquipments;

	UFUNCTION()
    void OnRep_RefreshEquipments();

	public:
	void SetRefreshEquipments(bool bRefresh);

		
	////////////////////////////////////////////////////////////////////////////
	//// 장착 아이템
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Weapon, BlueprintReadOnly, Category = Equipment)
	AWeapon* CurrentWeapon;

	UFUNCTION()
    void OnRep_Weapon();

public:
	void SetCurrentWeapon(bool bEquip);

public:
	void UseQuickItem();

	void EquipItem(FItemTable Item);
	void UnEquipItem(EItemFilter ItemFilter, int32 EquipIndex);

	void AddQuickItem(FItemTable Item, bool bShiftLeft);
	void AddQuickItemAt(FItemTable Item, int32 QuickIndex);
	
	void RemoveQuickItemAt(EItemFilter ItemFilter, int32 EquipIndex, bool bShiftLeft);

	void ShiftLeftEquipments(EMouseWheel MouseWheel);

protected:
	UFUNCTION(Server, Reliable)
    void ServerUseQuickItem();

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(FItemTable Item);
	
	UFUNCTION(Server, Reliable)
    void ServerUnEquipItem(EItemFilter ItemFilter, int32 EquipIndex);

	UFUNCTION(Server, Reliable)
    void ServerAddQuickItem(FItemTable Item, bool bShiftLeft);
	
	UFUNCTION(Server, Reliable)
	void ServerAddQuickItemAt(FItemTable Item, int32 QuickIndex);
	
	UFUNCTION(Server, Reliable)
    void ServerRemoveQuickItem(EItemFilter ItemFilter, int32 QuickIndex, bool bShiftLeft);

	UFUNCTION(Server, Reliable)
    void ServerChangeQuickSlot(EMouseWheel MouseWheel);

	UFUNCTION(NetMulticast, Reliable)
    void MulticastRefreshClients(UEquipmentComponent* EquipComponent);
	
	bool UseQuickInventoryItem();
	bool EquipInventoryItem(FItemTable Item);
	bool UnEquipEquipmentItem(EItemFilter ItemFilter, int32 EquipIndex);

	bool AddQuickEquipmentItem(FItemTable Item, bool bShiftLeft);
	void AddItemInEquipment(TArray<FItemTable>& QuickEquipments, FItemTable Item, bool bShiftLeft);
	bool AddQuickEquipmentItemAt(FItemTable Item, int32 EquipIndex);
	void AddItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, int32 EquipIndex, FItemTable Item);

	bool RemoveQuickEquipmentItemAt(EItemFilter ItemFilter, int32 EquipIndex, bool bShiftLeft);
	void RemoveItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, int32 EquipIndex, bool bShiftLeft);

	bool ShilftLeftEquipmentSlot(EItemFilter ItemFilter);
	void ShiftLeft(TArray<FItemTable>& QuickEquipments, EItemFilter ItemFilter);
	
public:
	FORCEINLINE bool IsWeaponEquip() const {return QuickWeapons.Num()>0;}
	FORCEINLINE float GetEquipWeight() const {return EquipWeight;}
	
	float GetWeaponDamage(EPlayerAttack PlayerAttack);
	
	UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FItemTable> GetQuickWeapons() const {return QuickWeapons ;}

	UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FItemTable> GetQuickShields() const {return QuickShields ;}

	UFUNCTION(BlueprintCallable)
    TArray<FText> GetDamageText (EItemFilter ItemFilter);
	
	UFUNCTION(BlueprintCallable)
	FText GetArmorText(bool bMeleeArmor);

	UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FItemTable> GetQuickPotions() const {return QuickPotions;}
	
	UFUNCTION(BlueprintCallable)
    FORCEINLINE TMap<int32, FItemTable> GetArmorMap() const {return ArmorMap;}

	UFUNCTION(BlueprintCallable)
    TArray<UTexture2D*> GetItemIconsByQuickIndex(EItemFilter ItemFilter) const ;

	UFUNCTION(BlueprintCallable)
	void HoverEquipmentSlot(EItemFilter ItemFilter, int32 EquipIndex);

	void UpdateItemDescription(TArray<FItemTable>& QuickEquipments, int32 EquipIndex);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
};