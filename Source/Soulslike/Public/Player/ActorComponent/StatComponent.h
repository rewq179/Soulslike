// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "StatComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

	FTimerHandle StaminaDrainTimer;
	FTimerHandle StaminaRecoveryTimer;

protected:
	/** 회복되는 스태미나 비율 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	float RecoveryRate;

	/** 감소 되는 비율 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	float DrainRate;

public:	
	UStatComponent();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	FPlayerStat PlayerStat;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

public:
	FOnHpChangedDelegate OnHpChanged;

	/** True : 스테미너 소모 타이머, False : 회복 타이머 */
	void PlayStaminaTimer(bool bDrain);
	
	/** 2개의 스테미너 타이머 모두 초기화 */
	void ClearStaminaTimers();

	void RecoveryStamina();
	void DrainStamina();

	void AddStaminaValue(float Value);
	void AddSoulsValue(float Value);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FPlayerStat GetPlayerStat() const {return PlayerStat;}

	FORCEINLINE float GetCurHp() const { return PlayerStat.CurHp; }
	FORCEINLINE float GetMaxHp() const { return PlayerStat.MaxHp; }
	FORCEINLINE float GetCurStamina() const { return PlayerStat.CurStamina; }
	FORCEINLINE float GetMaxStamina() const { return PlayerStat.MaxStamina; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetSoulsCount() const { return PlayerStat.SoulsCount; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
