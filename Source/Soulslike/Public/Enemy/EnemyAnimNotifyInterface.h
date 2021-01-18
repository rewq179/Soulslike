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
	void LightAttackAnimStart(bool bStart);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void HeavyAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void ChargeAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void RangeAttackAnimStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void HandAttackAnimStart(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
    void DeadAnimStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void PlayEffect(UParticleSystem* InParticle, USoundBase* InSound, const FTransform InTransform);
};
