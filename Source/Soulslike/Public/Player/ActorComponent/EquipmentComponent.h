// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "Chaos/AABB.h"

#include "EquipmentComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;
class AWeapon;

/**
* 용도: 플레이어의 장비 장착, 퀵 아이템 장착
*
* 무기, 방패, 포션을 장착 및 예비로 둘 수 있다.
* 플레이어는 마우스 휠을 아래로 내려, 퀵 아이템을 바꿀 수 있다.
*/

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
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon , BlueprintReadOnly, Category = Equipment)
	AWeapon* CurrentWeapon;

	UFUNCTION()
    void OnRep_CurrentWeapon();
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponHide, BlueprintReadOnly, Category = Equipment)
	bool bWeaponHide;

	UFUNCTION()
    void OnRep_WeaponHide();

public:
	void SetCurrentWeapon(AWeapon* InWeapon);
	void HideWeapon(const bool bHide);

	////////////////////////////////////////////////////////////////////////////
	//// 외부에서 실행되는 함수

	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void UseQuickItem();

	void EquipItem(FItemTable Item);
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void UnEquipItem(EItemFilter ItemFilter, int32 EquipIndex);
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void AddQuickItem(FItemTable Item, bool bShiftLeft);
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void AddQuickItemAt(FItemTable Item, int32 QuickIndex);
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void RemoveQuickItemAt(EItemFilter ItemFilter, int32 EquipIndex, bool bShiftLeft);

	void ShiftLeftEquipments(EMouseWheel MouseWheel);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 클라이언트가 서버에게 요청하는 함수
	
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

	////////////////////////////////////////////////////////////////////////////
	//// 멀티 캐스트

	/** 현재는 사용할 필요 없음. */
	UFUNCTION(NetMulticast, Reliable)
    void MulticastRefreshClients(UEquipmentComponent* EquipComponent);

	////////////////////////////////////////////////////////////////////////////
	//// 실제로 동작되는 함수
	
	bool UseQuickInventoryItem();
	bool EquipInventoryItem(FItemTable Item);
	bool UnEquipEquipmentItem(EItemFilter ItemFilter, int32 EquipIndex);

	bool AddQuickEquipmentItem(FItemTable Item, bool bShiftLeft);
	void AddItemInEquipment(TArray<FItemTable>& QuickEquipments, FItemTable Item, bool bShiftLeft);
	bool AddQuickEquipmentItemAt(FItemTable Item, int32 EquipIndex);
	void AddItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, int32 EquipIndex, FItemTable Item);

	bool RemoveQuickEquipmentItemAt(const EItemFilter ItemFilter, const int32 EquipIndex, const bool bShiftLeft);
	void RemoveItemInEquipmentAt(TArray<FItemTable>& QuickEquipments, const int32 EquipIndex, const bool bShiftLeft);

	bool ShiftLeftEquipmentSlot(const EItemFilter ItemFilter);
	void ShiftLeftArrayByFilter(TArray<FItemTable>& QuickEquipments, const EItemFilter ItemFilter);

	void AddEquipWeight(const float Weight);
	
public:
	////////////////////////////////////////////////////////////////////////////
	//// 개터
		
	FORCEINLINE bool IsWeaponEquip() const {return QuickWeapons.Num() > 0;}
	FORCEINLINE float GetEquipWeight() const {return EquipWeight;}
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    FORCEINLINE TMap<int32, FItemTable> GetArmorMap() const {return ArmorMap;}

	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    FORCEINLINE TArray<FItemTable> GetQuickWeapons() const {return QuickWeapons ;}

	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    FORCEINLINE TArray<FItemTable> GetQuickShields() const {return QuickShields ;}

	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    FORCEINLINE TArray<FItemTable> GetQuickPotions() const {return QuickPotions;}

	float GetWeaponDamage(const EPlayerAttack PlayerAttack);

	/** True: 물리 방어력, False: 마법 방어력을 출력한다. */
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    FText GetArmorText(const bool bMeleeArmor);

	/** 무기와 방패(장착: LightDamage, 미장착: 0)의 데미지 Texts를 출력한다. */
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    TArray<FText> GetDamageTexts (const EItemFilter ItemFilter);
	
	/** 장착된 장비의 아이콘들을 출력한다. */
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
    TArray<UTexture2D*> GetItemIconsByQuickIndex(const EItemFilter ItemFilter) const ;

	////////////////////////////////////////////////////////////////////////////
	//// HUD
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentComponent")
	void HoverEquipmentSlot(const EItemFilter ItemFilter, const int32 EquipIndex);

	void UpdateItemDescriptionByFilter(TArray<FItemTable>& QuickEquipments, const int32 EquipIndex) const;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
};
