// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"
#include "Player/ActorComponent/ActorComponentInterface.h"
#include "Player/PlayerAnimNotifyInterface.h"

#include "SoulCharacter.generated.h"

class ASoulPlayerController;
class UPlayerAnimInstance;
class UTargetComponent;
class UStatComponent;
class UInteractComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UComboComponent;

class AWeapon;
class AEnemy;
class UCameraComponent;
class USpringArmComponent;

class UAnimMontage;

UCLASS(config = Game)
class ASoulCharacter : public ACharacter, public IActorComponentInterface, public IPlayerAnimNotifyInterface
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animtaion, meta = (AllowPrivateAccess = "true"))
	UPlayerAnimInstance* PlayerAnimInstance;
	
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
	//// 사운드
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* BlockSound;

	float BaseTurnRate;
	float BaseLookUpRate;
	float MoveSpeed;

protected:
	/** True : 이동가능, False : 이동 불가능 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bMoveable;

	/** True : 캐릭터 및 카메라 이동 불가능, False : 가능*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bPlayingScene;

	UTargetComponent* TargetComponent;
	UInteractComponent* InteractComponent;
	UStatComponent* StatComponent;
	UInventoryComponent* InventoryComponent;
	UEquipmentComponent* EquipmentComponent;
	UComboComponent* ComboComponent;

public:
	ASoulCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;
	
	////////////////////////////////////////////////////////////////////////////
	//// 인터페이스

	// 애님 인스턴스
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void SetEquip(bool bEquip);
	virtual void SetEquip_Implementation(bool bEquip) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartLightAttackAnim();
    virtual void StartLightAttackAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void StartHeavyAttackAnim();
    virtual void StartHeavyAttackAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void EndAttackAnim();
    virtual void EndAttackAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void EndBlockAnim();
    virtual void EndBlockAnim_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
    void EndDeadAnim();
	virtual void EndDeadAnim_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimNotify Interface")
	void EndRollAnim();
	virtual void EndRollAnim_Implementation() override;
	
	// 상호작용
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact Interface")
	void SetInteractDoor(AInteractDoor* Door);
	virtual void SetInteractDoor_Implementation(AInteractDoor* Door) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact Interface")
    void SetPickUpActor(APickUpActor* Actor);
    virtual void SetPickUpActor_Implementation(APickUpActor* Actor) override;
	
	// 스텟 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stat Interface")
	UStatComponent* GetStatComponent();
	virtual UStatComponent* GetStatComponent_Implementation() override;
	
	// 인벤토리 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory Interface")
    UInventoryComponent* GetInventoryComponent();
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory Interface")
    TArray<FItemTable> GetInventory();
	virtual TArray<FItemTable> GetInventory_Implementation() override;
 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory Interface")
	void AddItem(FItemTable Item);
	virtual void AddItem_Implementation(FItemTable Item) override;

	// 장비 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment Interface")
	UEquipmentComponent* GetEquipmentComponent();
	virtual UEquipmentComponent* GetEquipmentComponent_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment Interface")
	float GetEquipWeight();
	virtual float GetEquipWeight_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Equipment Interface")
    void AddQuickItemAt(FItemTable Item, int32 EquipIndex);
	virtual void AddQuickItemAt_Implementation(FItemTable Item, int32 EquipIndex) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

	////////////////////////////////////////////////////////////////////////////
	//// 구르기
	
	/** True : 구르기중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Rolling, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bRolling;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float RollCost;

	void StartRoll(); // 스페이스바
    void EndRoll();
	
	UFUNCTION(Server, Reliable)
    void ServerRoll();

	UFUNCTION(NetMulticast, Reliable)
    void MulticastRoll();
	
	UFUNCTION()
	void OnRep_Rolling();

public:
	void SetRolling(bool bRoll);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 공격
	
	/** True : 공격중, False : 일반 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float LightAttackCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float HeavyAttackCost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float LightAttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float HeavyAttackRadius;

	/** Left or Right 마우스 입력 */
	void StartAttack(EPlayerAttack Attack);
	void EndAttack();

	UFUNCTION(Server, Reliable)
    void ServerAttack(EPlayerAttack Attack);

public:
	void SetAttacking(bool bAttack);
	
    void LightAttack(); // 좌클릭
    void HeavyAttack(); // 좌클릭 + 시프트키

	void CreateOverlapSphere(float AttackRadius); // LightAttack과 HeavyAttack 다른점은 OverlapSphere의 생성지점이다. 즉 AttackRadius만 달리하면된다.
	void ApplyDamageToActorInOverlapSphere(TArray<AActor*>& OverlappedActors, EPlayerAttack PlayerAttack);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 사망
	
	/** True : 죽음, False : 살아있음 */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = "Status")
	bool bDead;

	void EndDead() const; // 캐릭터 사망후 애니메이션 재생 금지 용도
	
	UFUNCTION()
	void OnPlayerStatHpChanged(float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser);

	UFUNCTION(Server, Reliable)
    void ServerDeath();

	UFUNCTION()
    void OnRep_Dead();

public:
	void HitReaction(float StunTime, FVector KnockBack, bool bKnockDown);
	
protected:
	////////////////////////////////////////////////////////////////////////////
	//// 블럭
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float BlockCost;

	/** True : 블럭중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Blocking, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bBlocking;

	void StartBlock(); // 마우스 우클릭
	void EndBlock();

	UFUNCTION(Server, Reliable)
    void ServerBlock(bool bBlock);

	UFUNCTION()
	void OnRep_Blocking();

public:
	/** 공격방향과 블럭방향이 너무 틀어져있으면 안됨. */
	UFUNCTION()
	bool IsBlocked(AActor* Enemy) const;

	void PlayBlockEffect();

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 액터 컴퍼넌트

	UFUNCTION(Server, Reliable)
    void ServerEquip(bool bEquip);

	UFUNCTION(NetMulticast, Reliable)
    void MulticastEquip(bool bEquip);
	
	UFUNCTION(Server, Reliable)
    void ServerInteractActor();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AActor* Target;

	/** True : 타게팅중, False : 일반  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bTargeting;

	/** True : 카메라 잠금, False, 카메라 잠금 해제 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bLockCamera;

	/** Tab 키 입력 */
	void StartTarget();

public:
	void SetLockCamera(AActor* InTarget, bool bLock);

	void AddSoulsValue(int32 Value);

protected:
	////////////////////////////////////////////////////////////////////////////
	//// 보스 몬스터
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AEnemy* BossEnemy;

public:
	void SetBossEnemy(AEnemy* InEnemy);

	UFUNCTION()
    void OnEnemyHpChanged(float CurHp, float MaxHp);

	void UseQuickPotion();
	void ShowMenuHUD();
	void BackMenuHUD();
	void TurnOffHUD();
	
	void ShiftLeftEquipments(EMouseWheel MouseWheel); // 무기, 방패, 포션 퀵 전환
	
	// 게터
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE bool IsRoll() const { return bRolling; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE int32 GetLightAttackLength() const { return LightAttackMontages.Num(); }

	bool IsEnoughStamina(const float Cost) const;
	
	// 세터
	FORCEINLINE void SetPlayerAnimInstance(UPlayerAnimInstance* AnimInstance) {PlayerAnimInstance = AnimInstance;}
	FORCEINLINE void SetMoveable(const bool bMove) { bMoveable = bMove; }
	FORCEINLINE void SetPlayingScene(const bool bScene) { bPlayingScene = bScene; }

	// 멀티 캐스트
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySound(USoundBase* Sound);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayParticle(UParticleSystem* Particle);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};