#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAnimNotifyInterface.h"
#include "DataType.h"

#include "Enemy.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class USphereComponent;
class UArrowComponent;
class AEnemyProjectile;
class UAnimationAsset;
class ASoulCharacter;
class USoundCue;
class UEnemyAnimInstance;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animtaion, meta = (AllowPrivateAccess = "true"))
	UEnemyAnimInstance* EnemyAnimInstance;
	
public:
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
	
	////////////////////////////////////////////////////////////////////////////
	//// 인터페이스

	// 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartLightAttackAnim();
	virtual void StartLightAttackAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartHeavyAttackAnim(float Radius, float Height, bool bKnockDown);
	virtual void StartHeavyAttackAnim_Implementation(float Radius, float Height, bool bKnockDown) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartChargeAttackAnim(float Radius, float Height, bool bKnockDown);
	virtual void StartChargeAttackAnim_Implementation(float Radius, float Height, bool bKnockDown) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartRangeAttackAnim();
	virtual void StartRangeAttackAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartHandAttackAnim(float Radius, float Height, bool bKnockDown);
	virtual void StartHandAttackAnim_Implementation(float Radius, float Height, bool bKnockDown) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
	void EndLightAttackAnim();
	virtual void EndLightAttackAnim_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
	void EndAggroAnim();
	virtual void EndAggroAnim_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void EndDeadAnim();
	virtual void EndDeadAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
	void PlayFootStepSound();
	virtual void PlayFootStepSound_Implementation() override;

	// 파티클 재생
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void PlayEffect(UParticleSystem* InParticle, USoundBase* InSound, const FTransform InTransform);
	virtual void PlayEffect_Implementation(UParticleSystem* InParticle, USoundBase* InSound, const FTransform InTransform) override;

protected:
	virtual void BeginPlay() override;
				
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

	/** 상태(대기, 걷기, 달리기)에 따라 이동속도가 달라진다 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EMovementState MovementState;

public:
	////////////////////////////////////////////////////////////////////////////
	//// 세터
	
	void SetMovementState(const EMovementState State);

	void SetTarget(AActor* InTarget);
	void ClearTarget();
	
	void SetLightCollision(bool bActive) const;

protected:
	/** Enemy의 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	FText Name;
	
	/** True: 보스, False: 일반 잡몹 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	bool bBossEnemy;

	/** 0: Light, 1: Heavy, 2: Charge, 3: Range, 4: None */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	EEnemyAttack EnemyAttack;

	/** 0: Light, 1: Heavy, 2: Charge, 3: Range */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	TArray<float> AttackDamages;

public:
	/** True: 원거리 공격 불가능, False: 가능 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bRangeDelay;

	/** 원거리 공격 쿨타임 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Status)
	float RangeDelayTime;

    void StartAggro(); // 어그로 애니메이션 재생
	void EndAggro();

	void StartAttack(EEnemyAttack Attack);
	void StartFirstAttack(int32 AttackNumber);
	void PlayAttackMontage(TArray<UAnimMontage*>& AttackMontages);
	
	void AttackEnd(float DelayTime); // BT와 연결된 델리게이트 실행

	////////////////////////////////////////////////////////////////////////////
	//// 공격 함수
	
	void HeavyAttack(float Radius, float Height, bool bKnockDown);
	void ChargeAttack(float Radius, float Height, bool bKnockDown);
	void CreateOverlapSphere(float Radius, float Height, bool bKnockDown); // OverlapSphere 생성
	
	void RangeAttack();

	void PlayEffectAtTransform(UParticleSystem* InParticle, USoundBase* InSound, FTransform Transform) const;

protected:
	////////////////////////////////////////////////////////////////////////////
	//// Hp 변화

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bDead;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 SoulsValue;

	UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(Server, Reliable)
	void ServerDeath(AActor* DamageCauser);

	void StartDead() const; // 죽은 뒤 애니메이션을 정지함

	void SetEnemyHpBarHUD(ASoulCharacter* Player);
	
public:
	FORCEINLINE FText GetEnemyName() const { return Name; }
	FORCEINLINE float GetCurHp() const { return CurHp; }
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetDetectDistance() const { return DetectDistance; }
	FORCEINLINE float GetMeleeDistance() const { return MeleeDistance; }
	FORCEINLINE float GetRangeDistance() const { return RangeDistance; }
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE bool IsBossEnemy() const { return bBossEnemy; }
	FORCEINLINE void SetEnemyAttack(EEnemyAttack Attack) { EnemyAttack = Attack; }
	FORCEINLINE void SetEnemyAnimInstance(UEnemyAnimInstance* AnimInstance) {EnemyAnimInstance = AnimInstance;}
	
	float GetDamage() const;

	/** Enemy의 체력이 변화하면 SoulPC에서 UMG 값 변경해줌 */
	FOnBossEnemyHpChangedDelegate OnBossHpChanged;
	FOnNormalEnemyHpChangedDelegate OnNormalHpChanged;

	/** Attack 애니메이션 종료시, BT에게 종료되었다는 정보를 준다. */
	FOnAggroMoitionEndDelegate OnAggroMoitionEnd;
	FOnLightAttackEndDelegate OnLightAttackEnd;
	FOnHeavyAttackEndDelegate OnHeavyAttackEnd;
	FOnRangeAttackEndDelegate OnRangeAttackEnd;
	FOnChargeAttackEndDelegate OnChargeAttackEnd;
	FOnChargeAttackEndDelegate OnFirstAttackEnd;

	UFUNCTION(BlueprintNativeEvent)
	void ShowTargetWidget(ASoulCharacter* InCharacter, bool bActive);

	UFUNCTION(BlueprintNativeEvent)
    void ShowHpBarWidget(bool bActive);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateHpPercent(const float Hp);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
