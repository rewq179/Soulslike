// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 용도: 플레이어와 적의 기반 AnimInstnace
 */

class USoundBase;

UCLASS()
class SOULSLIKE_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn)
	APawn* OwnerPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TArray<USoundBase*> FootStepSounds;

	/** 이동속도로 블랜드 스페이스에서 활용된다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	float MoveSpeed;

	/** 방향으로 BS에서 사용된다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	float Direction;
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void FootStep();

	void PlayerSound(const int32 SoundIndex);
};
