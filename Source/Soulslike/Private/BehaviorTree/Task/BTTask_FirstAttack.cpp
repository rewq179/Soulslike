// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_FirstAttack.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FirstAttack::UBTTask_FirstAttack()
{
	bNotifyTick = true;

	bFirstAttacking = false;
}

EBTNodeResult::Type UBTTask_FirstAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr || Enemy->bRangeDelay)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StartFirstAttack(AttackNumber);
	bFirstAttacking = true;
	Enemy->OnFirstAttackEnd.AddLambda([this]() -> void { bFirstAttacking = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_FirstAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bFirstAttacking) // delegate로 애니메이션 종료시 값이 false된다.
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::FirstAttack, true);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
