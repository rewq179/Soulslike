// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAggroMoition.generated.h"

/**
 * 용도 : 조종되고 있는 Pawn이 타겟을 조우하면 AggroMontage를 재생시켜준다.
 */

UCLASS()
class SOULSLIKE_API UBTTask_PlayAggroMoition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PlayAggroMoition();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bPlaying;
};
