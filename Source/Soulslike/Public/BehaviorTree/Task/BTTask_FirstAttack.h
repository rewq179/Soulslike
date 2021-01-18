// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FirstAttack.generated.h"

/**
 * 용도 : 조종되고 있는 Pawn의 첫번째 공격을 설정해주기 위함.
 */
UCLASS()
class SOULSLIKE_API UBTTask_FirstAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FirstAttack();

	/** ChargingAttackMontages에서 AttackNumber에 해당하는 공격을 재생한다. */
	UPROPERTY(EditAnywhere, Category = "Value")
	int32 AttackNumber;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bFirstAttacking;
};
