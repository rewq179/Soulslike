// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType.h"
#include "StatComponent.generated.h"

class ASoulCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHpChangedDelegate, UStatComponent*, StatComponent, float, Hp, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsDead;

	UPROPERTY(ReplicatedUsing = OnRep_Hp, BlueprintReadOnly, Category = "HpComponent")
	float Hp;

	UFUNCTION()
	void OnRep_Hp(float OldHp);


	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	float GetHp() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHpChangedDelegate OnHpChanged;

	UFUNCTION(BlueprintCallable, Category = "HpComponent")
	void Heal(float HealAmount);
};
