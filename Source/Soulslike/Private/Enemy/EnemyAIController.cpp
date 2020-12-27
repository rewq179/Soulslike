// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "Enemy/Enemy.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

const FName AEnemyAIController::Target(TEXT("Target"));
const FName AEnemyAIController::Distance(TEXT("Distance"));
const FName AEnemyAIController::PatternNumber(TEXT("PatternNumber"));
const FName AEnemyAIController::HomeLocation(TEXT("HomeLocation"));
const FName AEnemyAIController::PatrolLocation(TEXT("PatrolLocation"));
const FName AEnemyAIController::Dead(TEXT("Dead"));
const FName AEnemyAIController::Aggro(TEXT("Aggro"));

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomeLocation, InPawn->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("OnPossess : EnemyAI couln't Run!"));
		}
	}
}

void AEnemyAIController::StartBehaviorTree()
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BehaviorTreeComponent != nullptr)
	{
		BehaviorTreeComponent->StartTree(*BTAsset, EBTExecutionMode::Looped);
	}
}

void AEnemyAIController::StopBehaviorTree()
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BehaviorTreeComponent != nullptr)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
