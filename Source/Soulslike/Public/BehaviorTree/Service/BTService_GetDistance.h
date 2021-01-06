#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetDistance.generated.h"

/**
 * 용도 : 조종되고 있는 Pawn과 타겟의 거리차이를 구하기 위함.
 */

UCLASS()
class SOULSLIKE_API UBTService_GetDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) override;
};
