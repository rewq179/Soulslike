// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "System/CharacterAnimInstance.h"

#include "PlayerAnimInstance.generated.h"

/**
 * 용도: ABP에서 사용할 변수나 함수들을 모아놓음
 */

UCLASS()
class SOULSLIKE_API UPlayerAnimInstance : public UCharacterAnimInstance
{
	GENERATED_BODY()

protected:	
	/** True: 블럭중이며 FullBody를 한다. False: UpperBody를 사용한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bBlocking;

	/** True: 장착중이며 그에 맞는 블랜드스페이스를 사용한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bEquiped;

	/** True: 구르기중이며 FullBody를 한다. False: UpperBody를 사용한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bRolling;

	/** True: 타겟팅 중이며 블랜드 스페이스를 변경한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bTargeting;

public:
	virtual void NativeInitializeAnimation() override;
	
	FORCEINLINE void SetBlock(const bool bBlock) {bBlocking = bBlock;}
	FORCEINLINE void SetEquip(const bool bEquip) {bEquiped = bEquip;}
	FORCEINLINE void SetRoll(const bool bRoll) {bRolling = bRoll;}
	FORCEINLINE void SetTargeting(const bool bTarget) {bTargeting = bTarget;}
};
