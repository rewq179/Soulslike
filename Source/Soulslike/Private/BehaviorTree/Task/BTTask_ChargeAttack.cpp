// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_ChargeAttack.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChargeAttack::UBTTask_ChargeAttack()
{
	bNotifyTick = true; //

	bCharging = false;
}

EBTNodeResult::Type UBTTask_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StartAttack(EEnemyAttack::Enemy_ChargeAttack);
	bCharging = true;
	Enemy->OnChargeAttackEnd.AddLambda([this]() -> void { bCharging = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bCharging) // delegate로 애니메이션 종료시 값이 false된다.
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
