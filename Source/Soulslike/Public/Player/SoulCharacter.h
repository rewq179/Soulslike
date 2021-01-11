// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"
#include "Player/ActorComponent/ActorComponentInterface.h"

#include "SoulCharacter.generated.h"

class ASoulPlayerController;
class UTargetComponent;
class UStatComponent;
class UInteractComponent;
class UInventoryComponent;

class AWeapon;
class AEnemy;
class UCameraComponent;
class USpringArmComponent;

class UAnimMontage;

UCLASS(config = Game)
class ASoulCharacter : public ACharacter, public IActorComponentInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Class, meta = (AllowPrivateAccess = "true"))
	ASoulPlayerController* SoulPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	////////////////////////////////////////////////////////////////////////////
	//// 몽타주
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LightAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* BlockMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockDownMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockBackMontage;

	////////////////////////////////////////////////////////////////////////////
	//// 파티클

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SoulParticle;

	////////////////////////////////////////////////////////////////////////////
	//// 파티클
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* BlockSound;

	float BaseTurnRate;
	float BaseLookUpRate;

protected:
	/** True : 이동가능, False : 이동 불가능 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bMoveable;

	/** True : 캐릭터 및 카메라 이동 불가능, False : 가능*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bPlayingScene;

	/** 플레이어의 이동속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	float MoveSpeed;

public:
	ASoulCharacter();

	UTargetComponent* TargetComponent;
	UInteractComponent* InteractComponent;
	UStatComponent* StatComponent;
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActorComponent)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	////////////////////////////////////////////////////////////////////////////
	//// 인터페이스
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void SetInteractDoor(AInteractDoor* Door);
	virtual void SetInteractDoor_Implementation(AInteractDoor* Door) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void SetPickUpActor(APickUpActor* Actor);
    virtual void SetPickUpActor_Implementation(APickUpActor* Actor) override;
	
	// 스텟 컴포넌트

	// 인벤토리 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    UStatComponent* GetStatComponent();
	virtual UStatComponent* GetStatComponent_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	int32 GetSoulsCount();
	virtual int32 GetSoulsCount_Implementation() override;

	// 인벤토리 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    UInventoryComponent* GetInventoryComponent();
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void UseItem(int32 SlotIndex);
	virtual void UseItem_Implementation(int32 SlotIndex) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void UnEquipItem(int32 EquipIndex);
	virtual void  UnEquipItem_Implementation(int32 EquipIndex) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void SwapItem(int32 FromIndex, int32 ToIndex);
	virtual void SwapItem_Implementation(int32 FromIndex, int32 ToIndex) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void MoveItem(int32 FromIndex, int32 ToIndex);
	virtual void MoveItem_Implementation(int32 FromIndex, int32 ToIndex) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void RemoveItemAt(int32 SlotIndex, int32 Count);
	virtual void RemoveItemAt_Implementation(int32 SlotIndex, int32 Count) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void LockItemAt(int32 SlotIndex);
	virtual void LockItemAt_Implementation(int32 SlotIndex) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void SetQuickItemAt(int32 SlotIndex);
	virtual void SetQuickItemAt_Implementation(int32 SlotIndex) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void SortItem();
	virtual void SortItem_Implementation() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	/** Space Bar 입력 */
	void StartRoll();
	UFUNCTION(BlueprintCallable)
	void EndRoll();

	/** Left or Right 마우스 입력 */
	void StartAttack(EPlayerAttack Attack);
	
	UFUNCTION(BlueprintCallable)
    void EndAttack();
	
	/** V키 입력 */
	void StartBlock();
	UFUNCTION(BlueprintCallable)
	void EndBlock();

	/** Tab 키 입력 */
	void StartTarget();
	
	UFUNCTION(BlueprintCallable)
	void StartDead();

	void UseQuickPotion();
	void ShowMenuHUD();
	void TurnOffHUD();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

	////////////////////////////////////////////////////////////////////////////
	//// 구르기
	
	/** True : 구르기중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Rolling, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bRolling;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float RollCost;

	UFUNCTION()
	void OnRep_Rolling();

	UFUNCTION(BlueprintImplementableEvent, Category = Status)
	void OnUpdateRolling(bool bRoll);

public:
	void SetRolling(bool bRoll);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRoll();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRoll();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 공격
	
	/** True : 공격중, False : 일반 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float LightAttackCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float HeavyAttackCost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float LightAttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float HeavyAttackRadius;
	
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(EPlayerAttack Attack);

	UFUNCTION(BlueprintCallable)
    void LightAttack();

	UFUNCTION(BlueprintCallable)
    void HeavyAttack();
	
	void ApplyDamageToActorInOverlapSphere(TArray<AActor*>& OverlappedActors);
	
	////////////////////////////////////////////////////////////////////////////
	//// 상호 작용

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractActor();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 사망
	
	/** True : 죽음, False : 살아있음 */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = Stat)
	bool bDead;

	UFUNCTION()
    void OnRep_Dead();

	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
    void OnUpdateDeath();
	

	UFUNCTION()
	void OnHpChanged(float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser);

public:
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerDeath();

	void HitReaction(float StunTime, FVector KnockBack, bool bKnockDown);
	
protected:
	////////////////////////////////////////////////////////////////////////////
	//// 블럭
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float BlockCost;

	/** True : 블럭중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Blocking, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bBlocking;

	UFUNCTION()
	void OnRep_Blocking();

	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
    void OnUpdateBlock();

public:
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerBlock(bool bBlock);
	
	/** 공격방향과 블럭방향이 너무 틀어져있으면 안됨. */
	UFUNCTION()
	bool IsBlocked(AActor* Enemy) const;

	void PlayBlockEffect();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 보스 몬스터
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AEnemy* BossEnemy;

public:
	void SetBossEnemy(AEnemy* InEnemy);

	UFUNCTION()
	void OnEnemyHpChanged(float CurHp, float MaxHp);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 타겟팅
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AActor* Target;

	/** True : 타게팅중, False : 일반  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bTargeting;

	/** True : 카메라 잠금, False, 카메라 잠금 해제 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bLockCamera;

public:
	void SetLockCamera(AActor* InTarget, bool bLock);

	void AddSoulsValue(int32 Value);

	// 게터
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE bool IsRoll() const { return bRolling; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	// 세터
	FORCEINLINE void SetMoveable(const bool bMove) { bMoveable = bMove; }
	FORCEINLINE void SetPlayingScene(const bool bScene) { bPlayingScene = bScene; }

	// 멀티 캐스트
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlaySound(USoundBase* Sound);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayParticle(UParticleSystem* Particle);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};