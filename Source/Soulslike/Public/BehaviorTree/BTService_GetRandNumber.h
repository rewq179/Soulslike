// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetRandNumber.generated.h"

/**
 * �뵵 : ���� BT�� Ȯ���� ���� �����Ű�� ����.
 * ��� : 0~99�� �Ѱ��� ���ڸ� ����, Percents�� ���ԵǴ� Index�� ���� �ش� BT�� �����Ų��.
 */

UCLASS()
class SOULSLIKE_API UBTService_GetRandNumber : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetRandNumber();

	/** ���� BT�� �߻�Ȯ���� �ǹ��ϸ�, ������ 100�̿����Ѵ�. */
	UPROPERTY(EditAnywhere, Category = "Value")
	TArray<int32> Percents;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	bool GetRandNumber(int32& Result) const;
};
