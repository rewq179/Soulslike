// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "DataType.h"
#include "BTDecorator_CheckDistance.generated.h"

/**
 * 용도 : 조종되고 있는 Pawn과 Target에 거리에 따라 공격(라이트, 헤비, 차징, 원거리)이 가능한지 체크함
 */

class AEnemy;

UCLASS()
class SOULSLIKE_API UBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckDistance();

	/** 공격타입에 따른 사거리 체크에 사용됨 */
	UPROPERTY(EditAnywhere, Category = "Value")
	EMonsterAttack MonsterAttack;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
