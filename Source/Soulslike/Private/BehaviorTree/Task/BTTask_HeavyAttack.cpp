// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_HeavyAttack.h"


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_HeavyAttack::UBTTask_HeavyAttack()
{
	bNotifyTick = true;

	bAttacking = false;
}

EBTNodeResult::Type UBTTask_HeavyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<ASoulCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));
	if (Enemy == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	
	Enemy->StartAttack(EEnemyAttack::Enemy_HeavyAttack);
	bAttacking = true;
	Enemy->OnHeavyAttackEnd.AddLambda([this]() -> void { bAttacking = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_HeavyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bAttacking) // ������ ����Ǹ� ���� BT�� �����ų �� �ִ�.
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
