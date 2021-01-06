#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetPatrolLocation.generated.h"

/**
 * 용도 : Pawn의 정찰 지점(FVector)을 얻기 위함
 */

UCLASS()
class SOULSLIKE_API UBTTask_GetPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetPatrolLocation();

	/** Origin 위치에서 ± MaxRange만큼의 범위 지정 */
	UPROPERTY(EditAnywhere, Category = "Value")
	float MaxRange;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
