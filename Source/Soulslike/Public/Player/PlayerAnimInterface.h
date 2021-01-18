// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAnimInterface.generated.h"

/**
 * 용도 : SoulCharacter에서 바로 PlayerAnimInstance로 접근하기 위함
 */

UINTERFACE(MinimalAPI)
class UPlayerAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULSLIKE_API IPlayerAnimInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void SetEquip(bool bEquip);
};
