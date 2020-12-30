// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTDecorator_CheckDistance.h"

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

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::Target));

	if (Enemy == nullptr || Target == nullptr)
	{
		return false;
	}

	float Range;
	switch (MonsterAttack)
	{
	case EMonsterAttack::MATK_LightAttack:
		Range = Enemy->GetMeleeDistance();
		break;

	case EMonsterAttack::MATK_HeavyAttack:
		Range = Enemy->GetMeleeDistance();
		break;

	case EMonsterAttack::MATK_ChargeAttack:
		Range = Enemy->GetMeleeDistance();
		break;

	case EMonsterAttack::MATK_RangeAttack:
		Range = Enemy->GetRangeDistance();
		break;

	case EMonsterAttack::MATK_None:
		Range = Enemy->GetDetectDistance();
	}

	bResult = (Target->GetDistanceTo(Enemy) < Range);
	return bResult;
}