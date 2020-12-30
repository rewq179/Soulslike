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
	float RecoveryRate;
	float DrainRate;

public:	
	// Sets default values for this component's properties
	UStatComponent();

	/** 소유중인 Pawn */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	/** 소유중인 Pawn의 커스텀 Controller  */
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

	FORCEINLINE float GetCurHp() { return PlayerStat.CurHp; }
	FORCEINLINE float GetMaxHp() { return PlayerStat.MaxHp; }
	FORCEINLINE float GetCurStamina() { return PlayerStat.CurStamina; }
	FORCEINLINE float GetMaxStamina() { return PlayerStat.MaxStamina; }
	FORCEINLINE float GetSoulsCount() { return PlayerStat.SoulsCount; }

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
