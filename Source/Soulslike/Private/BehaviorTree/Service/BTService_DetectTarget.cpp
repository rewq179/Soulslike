// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Service/BTService_DetectTarget.h"

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

	// 조종되고 있는 폰, 세계, 
	AEnemy* Enemy = Cast<AEnemy>( OwnerComp.GetAIOwner()->GetPawn());
	UWorld* World = Enemy->GetWorld();
	if (Enemy == nullptr || World == nullptr )
	{
		return;
	}

	const FVector Center = Enemy->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, Enemy);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Enemy->GetDetectDistance()),
		CollisionQueryParam
	);

	if (bResult) // Overlap이 있으면?
	{
		for (auto OverlapResult : OverlapResults)
		{
			auto const Player = Cast<ASoulCharacter>(OverlapResult.GetActor());
			if (OverlapResult.GetActor()->ActorHasTag("Player") && Player) // 태그가 Player이며 Player로 캐스팅이 된다면?
			{
				if (Player->IsDead()) // 사망 : 블랙보드 및 타겟 초기화
				{
					ClearBlackBoard(OwnerComp);

					Enemy->ClearTarget();
				}

				else // 생존 : 블랙보드 및 타겟 설정
 				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::Target, Player);
						
					Enemy->SetTarget(Player);

					return;
				}
			}
		}
	}

	else // Overlap이 없음
	{
		ClearBlackBoard(OwnerComp);

		Enemy->ClearTarget();
	}
}

void UBTService_DetectTarget::ClearBlackBoard(UBehaviorTreeComponent& OwnerComp)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::Target, nullptr);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::Aggro, false);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::FirstAttack, false);
}
