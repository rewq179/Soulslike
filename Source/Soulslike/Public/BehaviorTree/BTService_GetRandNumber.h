// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetRandNumber.generated.h"

/**
 * 용도 : 하위 BT를 확률에 따라 실행시키기 위함.
 * 방법 : 0~99중 한개의 숫자를 고르고, Percents에 포함되는 Index를 통해 해당 BT를 실행시킨다.
 */

UCLASS()
class SOULSLIKE_API UBTService_GetRandNumber : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetRandNumber();

	/** 하위 BT의 발생확률을 의미하며, 총합은 100이여야한다. */
	UPROPERTY(EditAnywhere, Category = "Value")
	TArray<int32> Percents;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	bool GetRandNumber(int32& Result) const;
};
