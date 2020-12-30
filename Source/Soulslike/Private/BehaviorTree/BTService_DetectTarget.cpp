// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTService_DetectTarget.h"

#include "Player/SoulCharacter.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"


UBTService_DetectTarget::UBTService_DetectTarget()
{
	NodeName = TEXT("Detect Target");

	Interval = 1.0f;
}

void UBTService_DetectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	UWorld* World = ControllingPawn->GetWorld();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (ControllingPawn == nullptr || World == nullptr || Enemy == nullptr)
	{
		return;
	}

	FVector Center = ControllingPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Enemy->GetDetectDistance()),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor()->ActorHasTag("Player"))
			{
				if (auto const Player = Cast<ASoulCharacter>(OverlapResult.GetActor()))
				{
					if (Player->IsDead())
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::Target, nullptr);
						OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::Aggro, false);
						OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::FirstAttack, false);

						Enemy->ClearTarget();
					}

					else
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::Target, Player);
						
						Enemy->SetTarget(Player);

						return;
					}
				}
			}
		}
	}

	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::Target, nullptr);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::Aggro, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::FirstAttack, false);

		Enemy->ClearTarget();
	}
}