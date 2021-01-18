// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/BTTask_GetPatrolLocation.h"
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
	
	auto const ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); // 4.20부터는 ~~~V1으로 바뀜
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::HomeLocation);
	FVector PatrolLocation;
	if(NavSystem->K2_GetRandomReachablePointInRadius(GetWorld(), Origin, PatrolLocation,2000.f)) // 기존 GetRandom은 이제 구동안됨.
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::PatrolLocation, PatrolLocation);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


