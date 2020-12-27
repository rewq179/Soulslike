// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UArrowComponent;
class AEnemyProjectile;
class UAnimationAsset;
class ASoulCharacter;

/**
* Enemy�� ������ �����͵��� �������� ���� �� �ִϸ��̼ǵ��� ����Ѵ�.
* AI �о߸� ������ ��� ������, �Լ��� �ش� Ŭ������ ���Ѵ�.
*/


UCLASS()
class SOULSLIKE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimSequence *DeathAnim;

	FTimerHandle AttackTimer;
	FTimerHandle ChargeDelayTimer;
	FTimerHandle RangeDelayTimer;

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
	TArray<UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> RangeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> ChargeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy)
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy)
	TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy)
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

public:
	void SetMovementState(EMovementState State);
		
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** ���� ���� ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float MeleeDistance;

	/** ���Ÿ� ���� ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float RangeDistance;

public:
	void SetTarget(AActor* InTarget);

	UFUNCTION()
	void ClearTarget();

	UFUNCTION()
	void PlayAggroMotion();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EMonsterAttack MonsterAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TArray<float> AttackDamage;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bChargeDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bRangeDelay;

	FORCEINLINE void SetMonsterAttack(EMonsterAttack Attack) { MonsterAttack = Attack; }

	float GetDamage();
	
	UFUNCTION(BlueprintCallable)
	void SetMeleeCollision(bool bActive);

	UFUNCTION()
	void StartAttack(EMonsterAttack Attack, int32 AttackNumber);

	UFUNCTION()
	void SetAttackDelay(float WaitTime);

	UFUNCTION(BlueprintCallable)
	void HeavyAttack();

	UFUNCTION(BlueprintCallable)
	void RangeAttack();

	UFUNCTION(BlueprintCallable)
	void ChargeAttack();

	void CreateOverlapSphere(float Radius, float Damage, float Velocity);

protected:
	/** ������ ���� ü�� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float CurHp;

	/** ������ ���� ü�� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float MaxHp;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);


	/** ����� �÷��̾�� �� XP */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	int32 SoulsValue;

	//** True : ����, False : ������� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bDead;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath(AActor* DamageCauser);
	
	//** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Combat)
	void OnUpdateDeath();

public:
	FORCEINLINE bool IsDead() { return bDead; }
	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnAggroMoitionEndDelegate OnAggroMoitionEnd;

	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnLightAttackEndDelegate OnLightAttackEnd;

	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnHeavyAttackEndDelegate OnHeavyAttackEnd;

	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnRangeAttackEndDelegate OnRangeAttackEnd;

	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnChargeAttackEndDelegate OnChargeAttackEnd;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayAnimation(UAnimationAsset* AnimAsset, bool bLooping);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
