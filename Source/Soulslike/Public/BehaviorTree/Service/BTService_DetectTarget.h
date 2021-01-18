#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Enemy/Enemy.h"

#include "BTService_DetectTarget.generated.h"

/**
 * 용도 : DetectRange 안에 있는 플레이어를 찾기 위함.
 */

UCLASS()
class SOULSLIKE_API UBTService_DetectTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void ClearBlackBoard(UBehaviorTreeComponent& OwnerComp);
};
