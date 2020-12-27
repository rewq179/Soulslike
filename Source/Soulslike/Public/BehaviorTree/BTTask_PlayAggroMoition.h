// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAggroMoition.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API UBTTask_PlayAggroMoition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PlayAggroMoition();

	/** 몽타주 섹션 이름에 들어갈 숫자. Attack_? */
	UPROPERTY(EditAnywhere, Category = "Value")
	int32 AttackNumber;

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/** Pawn의 공격 애니메이션이 진행중인가? : True면 아직 진행중 */
	bool bPlaying;
};
