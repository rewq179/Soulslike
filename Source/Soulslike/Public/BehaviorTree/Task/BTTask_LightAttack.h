#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LightAttack.generated.h"

/**
 * 용도 : 조종되고 있는 Pawn의 LightAtatck 애니메이션을 재생/데미지 적용/상태이상 적용을 통합적으로 처리하기 위함
 */

UCLASS()
class SOULSLIKE_API UBTTask_LightAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LightAttack();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bAttacking;
};
