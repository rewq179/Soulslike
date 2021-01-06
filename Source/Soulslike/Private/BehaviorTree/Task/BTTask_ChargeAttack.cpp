// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_ChargeAttack.h"


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
	if (Enemy == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}


	Enemy->StartAttack(EEnemyAttack::Enemy_ChargeAttack);
	bCharging = true;
	Enemy->OnChargeAttackEnd.AddLambda([this]() -> void { bCharging = false; });

	// ��¡�� �ӵ� ���� ���� : Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->Status.FollowSpeed * 1.5f;

	return EBTNodeResult::InProgress;
}

void UBTTask_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bCharging) // ������ ����Ǹ� ���� BT�� �����ų �� �ִ�.
	{
		// ��¡�� �ӵ� ������ ���� �ӵ��� �������.

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
