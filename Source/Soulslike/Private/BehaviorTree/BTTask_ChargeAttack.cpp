// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_ChargeAttack.h"


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "Player/SoulCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ChargeAttack::UBTTask_ChargeAttack()
{
	bNotifyTick = true; //

	bCharging = false;
}

EBTNodeResult::Type UBTTask_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<ASoulCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));
	if (Enemy == nullptr || Target == nullptr || Enemy->bChargeDelay)
	{
		return EBTNodeResult::Failed;
	}


	Enemy->StartAttack(EMonsterAttack::MATK_ChargeAttack, AttackNumber);
	bCharging = true;
	Enemy->OnChargeAttackEnd.AddLambda([this]() -> void { bCharging = false; });

	// 차징시 속도 증가 사용시 : Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->Status.FollowSpeed * 1.5f;

	return EBTNodeResult::InProgress;
}

void UBTTask_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bCharging) // 진행이 종료되면 다음 BT를 실행시킬 수 있다.
	{
		// 차징시 속도 증가면 기존 속도로 해줘야함.

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
