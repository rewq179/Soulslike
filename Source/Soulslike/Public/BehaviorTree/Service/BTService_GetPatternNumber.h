#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetPatternNumber.generated.h"

/**
* 용도 : 하위 BT를 확률에 따라 실행시키기 위함.
* 방법 : 0~99중 한개의 숫자를 고르고, Percents에 포함되는 Index를 통해 해당 BT를 실행시킨다.
 */

UCLASS()
class SOULSLIKE_API UBTService_GetPatternNumber : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetPatternNumber();

	/** 패턴을 Percent 값에 따라 랜덤으로 구한다. */
	UPROPERTY(EditAnywhere, Category = "Value")
	TArray<int32> Percents;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	bool GetRandNumber(int32& Result) const;
};
