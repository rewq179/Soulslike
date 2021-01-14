// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()
	
	FTimerHandle ComboTimer;
	
	public:	
	// Sets default values for this component's properties
	UComboComponent();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();

	protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combo")
	bool bComboAttackable;
	
	/** 0~5까지로 콤보에 따른 가중치를 부여한다. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combo")
	int32 ComboCount;

	/** 최종 데미지 = 데미지 * ComboDamages(콤보 가중치) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<float> ComboDamages;

	/** 콤보 공격이 가능한 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	float ComboTime;

	public:
	void SetComboCount(bool bCombo);
	FORCEINLINE int32 GetComboCount() const {return ComboCount;}
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
