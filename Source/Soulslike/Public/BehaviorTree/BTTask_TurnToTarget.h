// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TurnToTarget.generated.h"

/**
 * 용도 : 공격 모션중 조종되고 있는 Pawn을 타겟을 바라보기 위함.
 */

UCLASS()
class SOULSLIKE_API UBTTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TurnToTarget();

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
