// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataType.generated.h"

class UTexture2D;
class ASoulCharacter;
class AWeapon;

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
//// 던전
///

USTRUCT(BlueprintType)
struct SOULSLIKE_API FDungeonRawTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Id;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnX_0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnY_0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnZ_0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnX_1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnY_1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnZ_1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnX_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnY_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnZ_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnX_3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnY_3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnZ_3;
	
};

USTRUCT(BlueprintType)
struct FDungeonTable
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Id;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> SpawnPoints;
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
	Item_Shield UMETA(DisplayName = "Shield"),
	Item_Helmet UMETA(DisplayName = "Helmet"),
	Item_Chest UMETA(DisplayName = "Chest"),
	Item_Gloves UMETA(DisplayName = "Gloves"),
	Item_Boots UMETA(DisplayName = "Boots"),
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
	FText Description;

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
	float MeleeArmor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MagicArmor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Icon;
};

////////////////////////////////////////////////////////////////////////////
//// 플레이어

UENUM(BlueprintType)
enum class EMouseWheel: uint8
{
	Wheel_Magic  UMETA(DisplayName = "Magic"), 
	Wheel_Weapon  UMETA(DisplayName = "Weapon"), 
	Wheel_Shield  UMETA(DisplayName = "Shield"), 
	Wheel_Potion  UMETA(DisplayName = "Potion") 
};

UENUM(BlueprintType)
enum class EPlayerAttack : uint8
{
	Player_LightAttack UMETA(DisplayName = "LightAttack"),
	Player_HeavyAttack UMETA(DisplayName = "HeavyAttack")
};


UENUM(BlueprintType)
enum class EHealthType : uint8
{
	Health_Hp UMETA(DisplayName = "Hp"),
	Health_Mp UMETA(DisplayName = "Mp"),
	Health_Stamina UMETA(DisplayName = "Stamina")
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

	/** 플레이어의 레벨 : 능력치를 찍을 수 있음 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Level = 1;

	/** 체력에 대한 수치 : Hp 양과 Hp 회복 속도, 최대 무게, 쇼크 및 출혈 저장 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Constitution = 5;

	/** 정신에 대한 수치 : 마법 방어력, Mp의 양과 회복 속도, 정신 공격(침묵, 수면, 공포)에 대한 저항 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Mentality = 5;
	
	/** 머리 회전과 응용력에 대한 수치 : 크리티컬 확률 및 데미지, 시전 속도, 디버프 저항(스텟에 대한) ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Witness = 5;
	
	/** 힘에 대한 수치 : 일반 공격과 스킬 데미지 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Strength = 5;
	
	/** 민첩에 대한 수치 : 이동 속도, 회피, 크리티컬 확률, 이동속도 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Dexterity = 5;
	
	/** 지식에 대한 수치 : 마법의 데미지, 마법의 크리티컬 데미지와 확률 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Intelligence = 5;
	
	/** 행운에 수치 : 아이템 드랍 ↑ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Luck = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurHp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurMp;
	
	/**  */	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxMp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurStamina;
	
	/**  */	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxStamina;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MeleeArmor = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MagicArmor = 0;
	
	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 SoulsCount = 0;
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
//// 위젯

UENUM(BlueprintType)
enum class EPlayerHUD : uint8
{
	Player_Inventory UMETA(DisplayName = "Inventory"),
	Player_Equipment UMETA(DisplayName = "Equipment"),
	Player_Status UMETA(DisplayName = "Status"),
	Player_Option UMETA(DisplayName = "Option"),
	Player_Key UMETA(DisplayName = "Key")
};

////////////////////////////////////////////////////////////////////////////
//// 델리게이트

// 몬스터
DECLARE_MULTICAST_DELEGATE(FOnAggroMoitionEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnLightAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHeavyAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnRangeAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChargeAttackEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNormalEnemyHpChangedDelegate, float, CurHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossEnemyHpChangedDelegate, float, CurHp, float, MaxHp);

// 플레이어
DECLARE_DELEGATE_OneParam(FMouseClickDelegate, EPlayerAttack);
DECLARE_DELEGATE_OneParam(FMouseWheelDelegate, EMouseWheel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHpChangedDelegate, float, Hp, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
