// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTService_GetRandNumber.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_GetRandNumber::UBTService_GetRandNumber()
{
	NodeName = TEXT("Get Rand Number");

	Interval = 1.0f;
	bNotifyBecomeRelevant = true;
}

void UBTService_GetRandNumber::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	int32 RandNumber = 0;

	if (GetRandNumber(RandNumber))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(AEnemyAIController::PatternNumber, RandNumber);
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Service_GetRandNumber : Number is over than 100"));
	}
}

bool UBTService_GetRandNumber::GetRandNumber(int32& Result) const
{
	int Rand = FMath::RandRange(0, 99);

	int High = 0;
	int Low = 0;

	for (int Index = 0; Index < Percents.Num(); Index++) // 20 30 50
	{
		High += Percents[Index];

		if (Low <= Rand && Rand <= High)
		{
			Result = Index;

			return true;
		}

		Low += Percents[Index];
	}

	return false;
}