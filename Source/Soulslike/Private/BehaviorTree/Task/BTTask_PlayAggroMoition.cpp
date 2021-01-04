// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_PlayAggroMoition.h"


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PlayAggroMoition::UBTTask_PlayAggroMoition()
{
	bNotifyTick = true;

	bPlaying = false;
}

EBTNodeResult::Type UBTTask_PlayAggroMoition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<ASoulCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));
	if (Enemy == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StartAggro();
	bPlaying = true;
	Enemy->OnAggroMoitionEnd.AddLambda([this]() -> void { bPlaying = false; });

	return EBTNodeResult::InProgress;
}

void UBTTask_PlayAggroMoition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying) // 진행이 종료되면 다음 BT를 실행시킬 수 있다.
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::Aggro, true);
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
