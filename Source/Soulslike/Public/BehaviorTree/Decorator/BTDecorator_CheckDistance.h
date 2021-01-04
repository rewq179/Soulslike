// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "DataType.h"
#include "BTDecorator_CheckDistance.generated.h"

/**
 * �뵵 : �����ǰ� �ִ� Pawn�� Target�� �Ÿ��� ���� ����(����Ʈ, ���, ��¡, ���Ÿ�)�� �������� üũ��
 */

class AEnemy;

UCLASS()
class SOULSLIKE_API UBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckDistance();

	/** ����Ÿ�Կ� ���� ��Ÿ� üũ�� ���� */
	UPROPERTY(EditAnywhere, Category = "Value")
	EMonsterAttack MonsterAttack;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
