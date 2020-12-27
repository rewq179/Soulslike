// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTService_GetDistance.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_GetDistance::UBTService_GetDistance()
{
	NodeName = TEXT("Get Distance");

	Interval = 1.0f;
}

void UBTService_GetDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));

	if (ControllingPawn == nullptr || Target == nullptr)
	{
		return;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(AEnemyAIController::Distance, Target->GetDistanceTo(ControllingPawn));
}
