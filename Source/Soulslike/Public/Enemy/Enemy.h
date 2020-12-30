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
class USoundCue;

/**
* Enemy�� ������ �����͵��� �������� ���� �� �ִϸ��̼ǵ��� ����Ѵ�.
* AI �о߸� ������ ��� ������, �Լ��� �ش� Ŭ������ ���Ѵ�.
*/


UCLASS()
class SOULSLIKE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	FTimerHandle AttackTimer;
	FTimerHandle RangeDelayTimer;

public:
	// Sets default values for this character's properties
	AEnemy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	USphereComponent* LightCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UArrowComponent* ProjectilePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* AggroMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> LightAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> RangeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	TArray<UAnimMontage*> ChargeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy)
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy)
	TSubclassOf<UDamageType> DamageType;

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

	/** Ž�� ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float DetectDistance;

	/** ���� ������ ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float MeleeDistance;

	/** ���Ÿ� ������ ������ �Ÿ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float RangeDistance;

public:
	void SetTarget(AActor* InTarget);

	UFUNCTION()
	void ClearTarget();

	UFUNCTION()
	void StartAggro();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EMonsterAttack MonsterAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TArray<float> AttackDamage;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bRangeDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float RangeDelayTime;

	UFUNCTION(BlueprintCallable)
	void SetLightCollision(bool bActive);

	UFUNCTION()
	void StartAttack(EMonsterAttack Attack, int32 AttackNumber = 0, bool bFirstAttack = false);

	UFUNCTION()
	void BrocastAttackEnd(float DelayTime);

	UFUNCTION(BlueprintCallable)
	void HeavyAttack(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintCallable)
	void RangeAttack();

	UFUNCTION(BlueprintCallable)
	void ChargeAttack(float Radius, float Height, bool bKnockDown);

	UFUNCTION(BlueprintCallable)
	void PlayEffectAtTransform(UParticleSystem* InParticle, USoundBase* InSound, FTransform Transform);

	void CreateOverlapSphere(float Radius, float Height, bool bKnockDown);

protected:
	/** ������ ���� ü�� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float CurHp;

	/** ������ ���� ü�� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float MaxHp;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

protected:
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
	FORCEINLINE float GetDetectDistance() { return DetectDistance; }
	FORCEINLINE float GetMeleeDistance() { return MeleeDistance; }
	FORCEINLINE float GetRangeDistance() { return RangeDistance; }
	FORCEINLINE float GetDamage() { return AttackDamage[(int)MonsterAttack]; }
	FORCEINLINE bool IsDead() { return bDead; }

	FORCEINLINE void SetMonsterAttack(EMonsterAttack Attack) { MonsterAttack = Attack; }

	/** Attack �ִϸ��̼� �����, BT���� ����Ǿ��ٴ� ������ �ش�. */
	FOnAggroMoitionEndDelegate OnAggroMoitionEnd;
	FOnLightAttackEndDelegate OnLightAttackEnd;
	FOnHeavyAttackEndDelegate OnHeavyAttackEnd;
	FOnRangeAttackEndDelegate OnRangeAttackEnd;
	FOnChargeAttackEndDelegate OnChargeAttackEnd;
	FOnChargeAttackEndDelegate OnFirstAttackEnd;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
