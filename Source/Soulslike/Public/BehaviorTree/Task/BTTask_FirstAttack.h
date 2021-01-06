// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FirstAttack.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API UBTTask_FirstAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FirstAttack();

	/** ��Ÿ�� ���� �̸��� �� ����. Attack_? */
	UPROPERTY(EditAnywhere, Category = "Value")
		int32 AttackNumber;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bAttacking;
};
