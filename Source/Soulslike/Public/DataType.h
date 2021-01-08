// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataType.generated.h"

class UTexture2D;
class ASoulCharacter;
class AWeapon;
class UTexture2D;

UENUM(BlueprintType)
enum class EApplyStat : uint8
{
	Stat_Hp UMETA(DisplayName = "Hp"),
	Stat_MaxHp UMETA(DisplayName = "MaxHp"),
	Stat_Stamina UMETA(DisplayName = "Stamina"),
	Stat_MaxStamina UMETA(DisplayName = "MaxStamina"),
	Stat_Damage UMETA(DisplayName = "Damage"),
	Stat_MoveSpeed UMETA(DisplayName = "MoveSpeed")
};

////////////////////////////////////////////////////////////////////////////
//// 아이템

UENUM(BlueprintType)
enum class EInteractType : uint8
{
	Interact_PickUp UMETA(DisplayName = "Interact"),
	Interact_Door UMETA(DisplayName = "Door"),
	Interact_Chest UMETA(DisplayName = "Chest")
};

UENUM(BlueprintType)
enum class EPickUpType : uint8
{
	Pick_Weapon UMETA(DisplayName = "Weapon"),
	Pick_Armor UMETA(DisplayName = "Armor"),
	Pick_Helmet UMETA(DisplayName = "Helmet"),
	Pick_Consume UMETA(DisplayName = "Consume"),
	Pick_Souls UMETA(DisplayName = "Souls")
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FPickUpInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EPickUpType PickUpType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PickUpId;
};

UENUM(BlueprintType)
enum class EItemFilter : uint8
{
	Filter_Weapon UMETA(DisplayName = "Sword"),
	Filter_Shield UMETA(DisplayName = "Shield"),
	Filter_Armor UMETA(DisplayName = "Armor"),
	Filter_Potion UMETA(DisplayName = "Potion")
};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Item_Sword UMETA(DisplayName = "Sword"),
	Item_Shield UMETA(DisplayName = "Sword"),
	Item_Chest UMETA(DisplayName = "Chest"),
	Item_Helmet UMETA(DisplayName = "Helmet"),
	Item_HpPotion UMETA(DisplayName = "HpPotion"),
	Item_MpPotion UMETA(DisplayName = "MpPotion"),
	Item_StaminaPotion UMETA(DisplayName = "StaminaPotion")
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FItemTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemFilter ItemFilter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Count;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxCount;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bLocked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bQuicked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Hp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Mp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LightDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HeavyDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Armor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Icon;
};

////////////////////////////////////////////////////////////////////////////
//// 플레이어

UENUM(BlueprintType)
enum class EPlayerAttack : uint8
{
	Player_LightAttack UMETA(DisplayName = "LightAttack"),
	Player_HeavyAttack UMETA(DisplayName = "HeavyAttack")
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FEquipInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEquipWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEquipShield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEquipArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEquipHelmet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEquipSouls;
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FPlayerStat
{
	GENERATED_USTRUCT_BODY()
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Armor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 SoulsCount;
};

////////////////////////////////////////////////////////////////////////////
//// 적

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	State_Idle UMETA(DisplayName = "Idle"),
	State_Walk UMETA(DisplayName = "Walk"),
	State_Run UMETA(DisplayName = "Run")
};

UENUM(BlueprintType)
enum class EEnemyAttack : uint8
{
	Enemy_LightAttack UMETA(DisplayName = "Light Attack"),
	Enemy_HeavyAttack UMETA(DisplayName = "Heavy Attack"),
	Enemy_ChargeAttack UMETA(DisplayName = "Charge Attack"),
	Enemy_RangeAttack UMETA(DisplayName = "Range Attack"),
	Enemy_None UMETA(DisplayName = "None")
};

////////////////////////////////////////////////////////////////////////////
//// 델리게이트

DECLARE_MULTICAST_DELEGATE(FOnAggroMoitionEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnLightAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHeavyAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnRangeAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChargeAttackEndDelegate);

DECLARE_DELEGATE_OneParam(FMouseClickDelegate, EPlayerAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHpChangedDelegate, float, Hp, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHpChangedDelegate, float, CurHp, float, MaxHp);
