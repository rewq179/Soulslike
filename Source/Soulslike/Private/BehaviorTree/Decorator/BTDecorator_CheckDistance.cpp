// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Decorator/BTDecorator_CheckDistance.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckDistance::UBTDecorator_CheckDistance()
{
	NodeName = TEXT("Check Distance");
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto const Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto const Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));

	if (Enemy == nullptr || Target == nullptr)
	{
		return false;
	}

	float Distance; // Melee, Range, None 총 3가지 거리 구분을 한다.
	if(EnemyAttack < EEnemyAttack::Enemy_RangeAttack)
	{
		Distance = Enemy->GetMeleeDistance();
	}

	else if(EnemyAttack == EEnemyAttack::Enemy_RangeAttack)
	{
		Distance = Enemy->GetRangeDistance();
	}

	else
	{
		Distance = Enemy->GetDetectDistance();
	}
	
	bResult = (Target->GetDistanceTo(Enemy) < Distance);
	return bResult;
}