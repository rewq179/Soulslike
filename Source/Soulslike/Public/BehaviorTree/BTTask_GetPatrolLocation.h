// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetPatrolLocation.generated.h"

/**
 * �뵵 : Pawn�� ���� ����(FVector)�� ��� ����
 */

UCLASS()
class SOULSLIKE_API UBTTask_GetPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetPatrolLocation();

	/** Origin ��ġ���� �� MaxRange��ŭ�� ���� ���� */
	UPROPERTY(EditAnywhere, Category = "Value")
		float MaxRange;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
