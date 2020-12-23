// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UArrowComponent;
class ASoulCharacter;
class AEnemyProjectile;

UCLASS()
class SOULSLIKE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	USphereComponent* MeleeCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UArrowComponent* LaunchPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* AggroMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> MeleeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* RangeAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* SpecialAttackMontage;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	/** ���� �ۼ�Ʈ*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AttackPercent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASoulCharacter> ClassFilter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** ������ �⺻ �̵��ӵ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float WalkSpeed;
	
	/** �޸��� �̵��ӵ� = WalkSpeed * SpeedMultiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SpeedMultiplier;

	/** ���� ����(���, �ȱ�, �޸���) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EMovementState MovementState;

	/** ������ ��ġ����, ���ư� ��ġ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector StartLocation;

	/** ���� ����(���, �ȱ�, �޸���) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bRoaming;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMovementState(EMovementState State);
		
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartRoaming();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	bool bFindTarget;

	/** ���� ���� ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float MeleeDistance;

	/** ���Ÿ� ���� ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float RangeDistance;

public:
	void SetTarget(AActor* InTarget);

	UFUNCTION(BlueprintCallable)
	void ClearTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FollowTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckAggro();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EMonsterAttack MonsterAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TArray<float> AttackDamage;

public:
	FORCEINLINE void SetMonsterAttack(EMonsterAttack Attack) { MonsterAttack = Attack; }

	float GetDamage();
	
	UFUNCTION(BlueprintCallable)
	void SetMeleeCollision(bool bActive);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void HeavyAttack();

	UFUNCTION(BlueprintCallable)
	void RangeAttack();

	UFUNCTION(BlueprintCallable)
	void SpecialAttack();

public:
	UFUNCTION()
	bool GetPercent(float Percent);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
