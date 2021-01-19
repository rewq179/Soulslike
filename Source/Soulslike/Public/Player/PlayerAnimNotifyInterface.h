// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAnimNotifyInterface.generated.h"

/**
 * 용도: 플레이어 애님인스턴스 함수를 SoulCharacter에서 사용하기 위함
 */

UINTERFACE(MinimalAPI)
class UPlayerAnimNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULSLIKE_API IPlayerAnimNotifyInterface
{
	GENERATED_BODY()

public:
	// 캐릭터에서 실행된다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void SetEquip(bool bEquip);

	// 애니메이션에서 애님 노티파이로 부터 실행된다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void StartLightAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void StartHeavyAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void EndBlockAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
    void EndAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
    void EndRollAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
    void EndDeadAnim();
};
