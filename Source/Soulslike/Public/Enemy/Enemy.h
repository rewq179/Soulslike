// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"
#include "Enemy.generated.h"

UCLASS()
class SOULSLIKE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 몬스터의 기본 이동속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float WalkSpeed;
	
	/** 달리기 이동속도 = WalkSpeed * SpeedMultiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SpeedMultiplier;

	/** 적의 상태(대기, 걷기, 달리기) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EMovementState MovementState;

	/** 스폰된 위치이자, 돌아갈 위치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector StartLocation;

	/** 적의 상태(대기, 걷기, 달리기) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bRoaming;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMovementState(EMovementState State);
		
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartRoaming();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	bool bFindTarget;

	void SetTarget(AActor* InTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FollowTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attack();
};
