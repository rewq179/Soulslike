// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"


#include "StatComponent.generated.h"

/**
* 용도: 플레이어의 스텟을 관리함
*
* 플레이어의 Health를 회복 시킨다.
* 
*/

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

	FTimerHandle StaminaRecoveryTimer;

public:	
	UStatComponent();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 타이머
	
	/** 회복되는 스태미나 비율 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	float RecoveryRate;
	
public:
	void PlayStaminaTimer();
	void RecoveryStamina();
	
	void ClearStaminaTimer();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 스텟 관련

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	FPlayerStat PlayerStat;

	UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

public:
	void AddStaminaValue(const float Value);
	void AddSoulsValue(const float Value);
	
	FOnHpChangedDelegate OnHpChanged;
	
	////////////////////////////////////////////////////////////////////////////
	//// 게터
	
	UFUNCTION(BlueprintCallable, Category = "StatComponent")
	FORCEINLINE FPlayerStat GetPlayerStat() const {return PlayerStat;}
	
	UFUNCTION(BlueprintCallable, Category = "StatComponent")
    FORCEINLINE int32 GetSoulsCount() const { return PlayerStat.SoulsCount; }
	
	FORCEINLINE float GetCurHp() const { return PlayerStat.CurHp; }
	FORCEINLINE float GetMaxHp() const { return PlayerStat.MaxHp; }
	FORCEINLINE float GetCurStamina() const { return PlayerStat.CurStamina; }
	FORCEINLINE float GetMaxStamina() const { return PlayerStat.MaxStamina; }

	UFUNCTION(BlueprintCallable, Category = "StatComponent")
	TArray<FText> GetStatText() const; //  Con, Men, Wit, Str, Dex, Int, Luck을 Text로 출력해줌

	UFUNCTION(BlueprintCallable, Category = "StatComponent")
	FText GetHealthText(const EHealthType HealthType) const; // Hp, Mp, Stamina를 Text로 출력해줌

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
