// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyAnimNotifyInterface.generated.h"

class UParticleSystem;
class USoundBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyAnimNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULSLIKE_API IEnemyAnimNotifyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void LightAttackAnimStart(bool bStart);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void HeavyAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void ChargeAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void RangeAttackAnimStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void HandAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void PlayEffect(UParticleSystem* InParticle, USoundBase* InSound, FTransform InTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim_Notify")
	void DeadAnimStart();
};
