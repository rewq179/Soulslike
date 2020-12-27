// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectTarget.generated.h"

/**
 * �뵵 : DetectRange �ȿ� �ִ� �÷��̾ ���� ����.
 */

UCLASS()
class SOULSLIKE_API UBTService_DetectTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectTarget();

	/** Ž�� ������ �Ÿ� */
	UPROPERTY(EditAnywhere, Category = "Value")
	float DetectRange;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
