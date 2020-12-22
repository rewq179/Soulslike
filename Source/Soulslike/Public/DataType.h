// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataType.generated.h"

class UTexture2D;
class ASoulCharacter;
class AWeapon;

UENUM(BlueprintType)
enum class EApplyStat : uint8
{
	STAT_Hp UMETA(DisplayName = "Hp"),
	STAT_MaxHp UMETA(DisplayName = "MaxHp"),
	STAT_Stamina UMETA(DisplayName = "Stamina"),
	STAT_MaxStamina UMETA(DisplayName = "MaxStamina"),
	STAT_Damage UMETA(DisplayName = "Damage"),
	STAT_MoveSpeed UMETA(DisplayName = "MoveSpeed")
};


USTRUCT(BlueprintType)
struct SOULSLIKE_API FUserProfile
{
	GENERATED_USTRUCT_BODY()

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText UserName;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 WeaponId;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText WeaponName;
	
	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* WeaponIcon;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponClass;
	
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Status;
};

////////////////////////////////////////////////////////////////////////////
//// 아이템

UENUM(BlueprintType)
enum class EInteractType : uint8
{
	INTERACT_PickUp UMETA(DisplayName = "Interact"),
	INTERACT_Door UMETA(DisplayName = "Door"),
	INTERACT_Chest UMETA(DisplayName = "Chest")
};

UENUM(BlueprintType)
enum class EPickUpType : uint8
{
	PICK_Weapon UMETA(DisplayName = "Weapon"),
	PICK_Souls UMETA(DisplayName = "Souls"),
	PICK_Armor UMETA(DisplayName = "Armor")
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FPickUpInfo
{
	GENERATED_USTRUCT_BODY()

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Name;

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EPickUpType PickUpType;
	
	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Armor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BlockValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 BlendSpaceInt;
};

////////////////////////////////////////////////////////////////////////////
//// 플레아어

UENUM(BlueprintType)
enum class EPlayerAttack : uint8
{
	PATK_LightAttack UMETA(DisplayName = "LightAttack"),
	PATK_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	PATK_Blocking UMETA(DisplayName = "Blocking")
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FEquipInfo
{
	GENERATED_USTRUCT_BODY()

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bWeaponEquiped;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bShieldEquiped;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bSoulsEquiped;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bArmorEquiped;
};

USTRUCT(BlueprintType)
struct SOULSLIKE_API FPlayerStat
{
	GENERATED_USTRUCT_BODY()
		
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurHp;
	

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHp;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurStamina;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxStamina;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Armor;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 SoulsCount;
};

////////////////////////////////////////////////////////////////////////////
//// 몬스터

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	STATE_Idle UMETA(DisplayName = "Idle"),
	STATE_Walk UMETA(DisplayName = "Walk"),
	STATE_Run UMETA(DisplayName = "Run")
};
