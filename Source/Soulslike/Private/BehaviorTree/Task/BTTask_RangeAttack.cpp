// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_RangeAttack.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RangeAttack::UBTTask_RangeAttack()
{
	bNotifyTick = true;

	bAttacking = false;
}

EBTNodeResult::Type UBTTask_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr || Enemy->bRangeDelay)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StartAttack(EEnemyAttack::Enemy_RangeAttack);
	bAttacking = true;
	Enemy->OnRangeAttackEnd.AddLambda([this]() -> void { bAttacking = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_RangeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bAttacking) // delegate로 애니메이션 종료시 값이 false된다.
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
