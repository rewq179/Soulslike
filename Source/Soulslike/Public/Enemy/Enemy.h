#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAnimNotifyInterface.h"
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
* Enemy가 보유한 데이터들을 바탕으로 전투 및 애니메이션들을 재생한다.
* AI 분야를 제외한 모든 데이터, 함수가 해당 클래스에 속한다.
*/

UCLASS()
class SOULSLIKE_API AEnemy : public ACharacter, public IEnemyAnimNotifyInterface
{
	GENERATED_BODY()

	FTimerHandle AttackTimer;
	FTimerHandle RangeDelayTimer;

public:
	// Sets default values for this character's properties
	AEnemy();

	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* WeaponMesh;
	
	/**  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	USphereComponent* LightCollision;
	
	/**  */
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
	virtual void BeginPlay() override;
				
protected:
	/** 몬스터의 기본 이동속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float WalkSpeed;

	/** 달리기 이동속도 = WalkSpeed * SpeedMultiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SpeedMultiplier;

	/** 탐지 가능한 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float DetectDistance;

	/** 근접 공격이 가능한 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float MeleeDistance;

	/** 원거리 공격이 가능한 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Target)
	float RangeDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** 적의 상태(대기, 걷기, 달리기) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EMovementState MovementState;

public:
	void SetMovementState(EMovementState State);

	void SetTarget(AActor* InTarget);

	UFUNCTION()
	void ClearTarget();

	UFUNCTION()
	void StartAggro();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void LightAttackAnimStart(bool bStart);
	virtual void LightAttackAnimStart_Implementation(bool bStart) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void HeavyAttackAnimStart(float Radius, float Height, bool bKnockDown);
	virtual void HeavyAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown) override;


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void ChargeAttackAnimStart(float Radius, float Height, bool bKnockDown);
	virtual void ChargeAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void RangeAttackAnimStart();
	virtual void RangeAttackAnimStart_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void HandAttackAnimStart(float Radius, float Height, bool bKnockDown);
	virtual void HandAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void PlayEffect(UParticleSystem* InParticle, USoundBase* InSound, FTransform InTransform);
	virtual void PlayEffect_Implementation(UParticleSystem* InParticle, USoundBase* InSound, FTransform InTransform) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim_Notify")
	void DeadAnimStart();
	virtual void DeadAnimStart_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	bool bBossEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	EEnemyAttack EnemyAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	TArray<float> AttackDamage;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bRangeDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	float RangeDelayTime;

	void SetLightCollision(bool bActive);

	UFUNCTION()
	void StartAttack(EEnemyAttack Attack, int32 AttackNumber = 0, bool bFirstAttack = false);

	UFUNCTION()
	void AttackEnd(float DelayTime);

	void HeavyAttack(float Radius, float Height, bool bKnockDown);

	void ChargeAttack(float Radius, float Height, bool bKnockDown);

	void ApplyDamageToActorInOverlapSphere(TArray<AActor*>& OverlappedActors, float Height, bool bKnockDown);

	void RangeAttack();

	void PlayEffectAtTransform(UParticleSystem* InParticle, USoundBase* InSound, FTransform Transform);


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	float CurHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	float MaxHp;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	int32 SoulsValue;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bDead;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath(AActor* DamageCauser);

	void StartDead();

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Status)
	void OnUpdateDeath();

public:
	FORCEINLINE FText GetEnemyName() const { return Name; }
	FORCEINLINE float GetCurHp() const { return CurHp; }
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetDetectDistance() const { return DetectDistance; }
	FORCEINLINE float GetMeleeDistance() const { return MeleeDistance; }
	FORCEINLINE float GetRangeDistance() const { return RangeDistance; }
	FORCEINLINE float GetDamage() const { return AttackDamage[(int)EnemyAttack]; }
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE bool IsBossEnemy() const { return bBossEnemy; }

	FORCEINLINE void SetMonsterAttack(EEnemyAttack Attack) { EnemyAttack = Attack; }

	/** Enemy의 체력이 변화하면 SoulPC에서 UMG 값 변경해줌 */
	FOnEnemyHpChangedDelegate OnEnemyHpChanged;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnAggroMoitionEndDelegate OnAggroMoitionEnd;
	FOnLightAttackEndDelegate OnLightAttackEnd;
	FOnHeavyAttackEndDelegate OnHeavyAttackEnd;
	FOnRangeAttackEndDelegate OnRangeAttackEnd;
	FOnChargeAttackEndDelegate OnChargeAttackEnd;
	FOnChargeAttackEndDelegate OnFirstAttackEnd;

	/** True : WB_Targeting Off, False : WB_Targeting On */
	void ToggleTargetWidget(ASoulCharacter* InCharacter, bool bHide);
	
	UFUNCTION(BlueprintNativeEvent)
	void ShowTargetWidget(ASoulCharacter* InCharacter, bool bHide);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
