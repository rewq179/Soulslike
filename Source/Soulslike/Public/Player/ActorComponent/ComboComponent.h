// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboComponent.generated.h"

/**
* 용도: 플레이어의 콤보 공격을 관리하기 위함
*
* 라이트 공격(마우스 좌클릭)에 따라 ComboCount가 올라간다.
* 시간내 재입력시 ComboCount에 해당하는 Light Attack Montages가 실행된다.
* 시간이 오버되면 초기화한다.
*/

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()
	
	FTimerHandle ComboTimer;

public:	
	UComboComponent();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize(); // Owner 설정 및 초기 설정

protected:
	/** 콤보의 최대치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 ComboMaxCount;
	
	/** 0~5까지로 콤보에 따른 가중치를 부여한다. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 ComboCount;

	/** 최종 데미지 = 데미지 * ComboDamages(콤보 가중치) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<float> ComboDamages;

	/** 최종 데미지 = 데미지 * ComboDamages(콤보 가중치) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<float> ComboCosts;

	/** 콤보 공격이 가능한 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	float ComboTime;

public:
	void AddComboCount(const bool bReset); // ComboCount가 Max를 초과하거나, 리셋시 0이 된다.
	void StartComboTimer();
	void ClearComboTimer();

	/** 콤보가 맥스를 넘을 경우 0을, 나머지는 ComboCount를 가져온다. */
	int32 GetComboCount(); 
	FORCEINLINE float GetComboDamage() {return ComboDamages[ComboCount];}
	FORCEINLINE float GetComboCost() {return ComboCosts[ComboCount];}
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
