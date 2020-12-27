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
* Enemy가 보유한 데이터들을 바탕으로 전투 및 애니메이션들을 재생한다.
* AI 분야를 제외한 모든 데이터, 함수가 해당 클래스에 속한다.
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

	/** 몬스터의 기본 이동속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float WalkSpeed;
	
	/** 달리기 이동속도 = WalkSpeed * SpeedMultiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SpeedMultiplier;

	/** 적의 상태(대기, 걷기, 달리기) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EMovementState MovementState;

public:
	void SetMovementState(EMovementState State);
		
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** 근접 공격 가능한 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float MeleeDistance;

	/** 원거리 공격 가능한 거리 */
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
	/** 몬스터의 현재 체력 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float CurHp;

	/** 몬스터의 현재 체력 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float MaxHp;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);


	/** 사망시 플레이어에게 줄 XP */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	int32 SoulsValue;

	//** True : 죽음, False : 살아있음 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bDead;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath(AActor* DamageCauser);
	
	//** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Combat)
	void OnUpdateDeath();

public:
	FORCEINLINE bool IsDead() { return bDead; }
	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnAggroMoitionEndDelegate OnAggroMoitionEnd;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnLightAttackEndDelegate OnLightAttackEnd;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnHeavyAttackEndDelegate OnHeavyAttackEnd;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnRangeAttackEndDelegate OnRangeAttackEnd;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnChargeAttackEndDelegate OnChargeAttackEnd;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayAnimation(UAnimationAsset* AnimAsset, bool bLooping);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
