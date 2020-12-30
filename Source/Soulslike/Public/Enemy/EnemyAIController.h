// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * Behavior Tree���� ���� Blackboard�� BehaviorTree�� ��Ƴ��Ҵ�.
 * ���͵��� ������ BP_EnemyAI�� �����Ѵ�.
 */

UCLASS()
class SOULSLIKE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController() {}

	static const FName Target;
	static const FName Distance;
	static const FName PatternNumber;
	static const FName HomeLocation;
	static const FName PatrolLocation;
	static const FName Dead;
	static const FName Aggro;
	static const FName FirstAttack;

protected:
	/** ���� ������ ���� */
	UPROPERTY(EditDefaultsOnly, Category = "BT")
	class UBlackboardData* BBAsset;

	/** ���� �൵Ʈ�� ���� */
	UPROPERTY(EditDefaultsOnly, Category = "BT")
	class UBehaviorTree* BTAsset;

public:
	virtual void OnPossess(APawn* InPawn) override; // 4.22���ʹ� OnPosses��. ������ Possess�� ����ؾ��Ѵ�. �ƴϸ� GetPawn()���� Pawn�� ���� �� ����.

	void StartBehaviorTree();

	void StopBehaviorTree();
};
