// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Service/BTService_GetPatternNumber.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_GetPatternNumber::UBTService_GetPatternNumber()
{
	NodeName = TEXT("Get Rand Number");

	Interval = 1.0f;
	bNotifyBecomeRelevant = true;
}

void UBTService_GetPatternNumber::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

bool UBTService_GetPatternNumber::GetRandNumber(int32& Result) const
{
	const int Rand = FMath::RandRange(0, 99);

	int High = 0;

	// 계산 방식 : 20, 30, 50 일때 Num < 20, Num< 50, Num< 100 이런식으로 Index(패턴 번호)를 리턴함.
	for (int Index = 0; Index < Percents.Num(); ++Index) 
	{
		High += Percents[Index];

		if (Rand < High)
		{
			Result = Index;

			return true;
		}
	}

	return false;
}