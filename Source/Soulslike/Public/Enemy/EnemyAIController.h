#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
* Behavior Tree에서 사용될 Blackboard와 BehaviorTree를 모아놓았다.
* 몬스터들은 각각의 BP_EnemyAI를 소유한다.
*/

UCLASS()
class SOULSLIKE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController() {}

	static const FName Target;
	static const FName Distance;
	static const FName PatternNumber;
	static const FName HomeLocation;
	static const FName PatrolLocation;
	static const FName Dead;
	static const FName Aggro;
	static const FName FirstAttack;

protected:
	/** 사용될 블랙보드 에셋 */
	UPROPERTY(EditDefaultsOnly, Category = "BT")
	class UBlackboardData* BBAsset;

	/** 사용될 행도트리 에셋 */
	UPROPERTY(EditDefaultsOnly, Category = "BT")
	class UBehaviorTree* BTAsset;

public:
	virtual void OnPossess(APawn* InPawn) override; 

	void StartBehaviorTree() const;
	void StopBehaviorTree() const;
};
