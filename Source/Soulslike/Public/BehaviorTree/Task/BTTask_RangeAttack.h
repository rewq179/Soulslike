// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RangeAttack.generated.h"

/**
 * �뵵 : �����ǰ� �ִ� Pawn�� AreaAttack �ִϸ��̼��� ���/������ ����/�����̻� ������ ���������� ó���ϱ� ����
 */
UCLASS()
class SOULSLIKE_API UBTTask_RangeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RangeAttack();
	
protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/** Pawn�� ���� �ִϸ��̼��� �������ΰ�? : True�� ���� ������ */
	bool bAttacking;
};
