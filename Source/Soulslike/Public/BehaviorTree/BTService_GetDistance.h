// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetDistance.generated.h"

/**
 * �뵵 : �����ǰ� �ִ� Pawn�� Ÿ���� �Ÿ����̸� ���ϱ� ����.
 */

UCLASS()
class SOULSLIKE_API UBTService_GetDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) override;
};
