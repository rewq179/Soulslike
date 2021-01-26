#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyAnimNotifyInterface.generated.h"

/**
* 용도: 애니메이션 Notify에서 Enemy의 함수를 바로 실행시키기 위함
*/

class UParticleSystem;
class USoundBase;

UINTERFACE(MinimalAPI)
class UEnemyAnimNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULSLIKE_API IEnemyAnimNotifyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StartLightAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StartHeavyAttackAnim(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StartChargeAttackAnim(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StartRangeAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StartHandAttackAnim(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
    void EndLightAttackAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void EndAggroAnim();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
    void EndDeadAnim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void PlayFootStepSound();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void PlayEffect(UParticleSystem* InParticle, USoundBase* InSound, const FTransform InTransform);
};
