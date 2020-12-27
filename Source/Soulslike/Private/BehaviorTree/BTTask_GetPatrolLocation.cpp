// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_GetPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "NavigationSystem.h"

UBTTask_GetPatrolLocation::UBTTask_GetPatrolLocation()
{
	NodeName = TEXT("GetPatrolLocation");
}

EBTNodeResult::Type UBTTask_GetPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); // 4.20���ʹ� ~~~V1���� �ٲ�
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::HomeLocation);
	//FNavLocation NextPatrol;

	FVector NextLocation = NavSystem->GetRandomReachablePointInRadius(GetWorld(), Origin, 2000.f);
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::PatrolLocation, NextLocation);

		return EBTNodeResult::Succeeded;
	}

	//if (NavSystem->GetRandomPointInNavigableRadius(Origin, MaxRange, NextPatrol)) // origin��ġ���� ��MaxRange�� ������ ���� ����Ʈ�� ã�ڴ�.
	//{
	//	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::PatrolLocation, NextPatrol.Location);

	//	return EBTNodeResult::Succeeded;
	//}

	return EBTNodeResult::Failed;
}


