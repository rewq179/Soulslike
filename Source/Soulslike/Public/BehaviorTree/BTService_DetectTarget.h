// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectTarget.generated.h"

/**
 * 용도 : DetectRange 안에 있는 플레이어를 참기 위함.
 */

UCLASS()
class SOULSLIKE_API UBTService_DetectTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectTarget();

	/** 탐지 가능한 거리 */
	UPROPERTY(EditAnywhere, Category = "Value")
	float DetectRange;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
