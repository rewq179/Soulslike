// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_LightAttack.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_LightAttack::UBTTask_LightAttack()
{
	bNotifyTick = true;

	bAttacking = false;
}

EBTNodeResult::Type UBTTask_LightAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<ASoulCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));
	if (Enemy == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StartAttack(EMonsterAttack::MATK_LightAttack);
	bAttacking = true;
	Enemy->OnLightAttackEnd.AddLambda([this]() -> void { bAttacking = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_LightAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bAttacking) // 진행이 종료되면 다음 BT를 실행시킬 수 있다.
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}