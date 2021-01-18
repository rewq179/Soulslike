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
const FName AEnemyAIController::FirstAttack(TEXT("FirstAttack"));

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomeLocation, InPawn->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("OnPossess : EnemyAI Couln't Run!"));
		}
	}
}

void AEnemyAIController::StartBehaviorTree() const
{
	if (const auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StartTree(*BTAsset, EBTExecutionMode::Looped);
	}
}

void AEnemyAIController::StopBehaviorTree() const
{
	if (const auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
