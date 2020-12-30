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

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/** Pawn�� ���� �ִϸ��̼��� �������ΰ�? : True�� ���� ������ */
	bool bPlaying;
};
