// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "PlayerAnimInstance.generated.h"

/**
 * 용도: 플레이어의 ABP를 C++에서 구현하기 위함
 */

UCLASS()
class SOULSLIKE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn)
	APawn* Pawn;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	float MoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bBlocking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bEquiped;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bRolling;

public:
	FORCEINLINE void SetBlock(bool bBlock) {bBlocking = bBlock;}
	FORCEINLINE void SetEquip(bool bEquip) {bEquiped = bEquip;}
	FORCEINLINE void SetRoll(bool bRoll) {bRolling = bRoll;}
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
