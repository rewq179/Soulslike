// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataType.generated.h"

class UTexture2D;
class ASoulCharacter;
class AWeapon;

UENUM(BlueprintType)
enum class EPlayerAttack : uint8
{
	PATK_LightAttack UMETA(DisplayName = "LightAttack"),
	PATK_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	PATK_Blocking UMETA(DisplayName = "Blocking")
};

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