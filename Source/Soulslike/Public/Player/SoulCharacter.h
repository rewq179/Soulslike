// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"

#include "SoulCharacter.generated.h"

class ASoulPlayerController;
class UTargetComponent;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class AWeapon;
class APickUpActor;
class UAnimMontage;
class UDamageType;
class USoundCue;

UCLASS(config = Game)
class ASoulCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Ŀ���� �÷��̾� ��Ʈ�ѷ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Class, meta = (AllowPrivateAccess = "true"))
	ASoulPlayerController* SoulPC;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LightAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimSequence *DeathAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* BlockMontage;

	/** ���� �� ������ ��ƼŬ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockParticle;

	/** �ҿ��� ������ �� ������ ��ƼŬ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SoulParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* BlockSound;
	   
	FTimerHandle StaminaDrainTimer;
	FTimerHandle StaminaRecoveryTimer;

	/** ���� �ӵ� */
	float MoveSpeed;
	
	/** ���� ���� �ӵ� */
	float SprintSpeed;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	float BaseLookUpRate;

public:
	/** Constructor */
	ASoulCharacter();

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTargetComponent* TargetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	DECLARE_DELEGATE_OneParam(FMouseClickDelegate, EPlayerAttack);

protected:
	/** True : �̵�����, False : �̵� �Ұ��� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bMoveable;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	/** True : ���׹̳� �Ҹ� Ÿ�̸�, False : ȸ�� Ÿ�̸� */
	void PlayStaminaTimer(bool bDrain);
	
	/** 2���� ���׹̳� Ÿ�̸� ��� �ʱ�ȭ */
	void ClearStaminaTimers();

	/** ���� ����ƮŰ �Է� */
	void StartSprint();

	/** ���� ����ƼŰ ���� */
	void EndSprint();

	/** �����̽��� �Է�  */
	void StartRoll();

	/** VŰ �Է� */
	void StartBlock();

	/** VŰ ���� */
	UFUNCTION(BlueprintCallable)
	void EndBlock();

	/** TŰ �Է� */
	void StartTarget();
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

protected:
	/** True : ��������(�̵��ӵ� 800), False : �Ϲ�(�̵����� 500) */
	UPROPERTY(ReplicatedUsing = OnRep_Sprinting, BlueprintReadOnly, Category = Movement)
	bool bSprinting;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool bSprint);

	/** RefNotify */
	UFUNCTION()
	void OnRep_Sprinting();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void OnUpdateSprinting();

public:
	void Sprint(bool bSprint);
	void SetSprinting(bool bSprint);
	
	void SetMaxWalkSpeed(bool bSprint);
	void RecoveryStamina();
	void DrainStamina();


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float RollCost;

	/** True : ��������, False : �Ϲ�  */
	UPROPERTY(ReplicatedUsing = OnRep_Rolling, BlueprintReadOnly, Category = Movement)
	bool bRolling;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Rolling();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void OnUpdateRolling(bool bRoll);


public:
	void SetRolling(bool bRoll);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRoll();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRoll();

	UFUNCTION(BlueprintCallable)
	void EndRoll();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EPlayerAttack PlayerAttack;

	/** True : ������, False : �Ϲ� */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Movement)
	bool bAttacking;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTryToActivateAbility(EPlayerAttack Attack);
	
	UFUNCTION(BlueprintCallable)
	void EndAttack();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PickUpActor)
	APickUpActor* CurrentPickUpActor;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractActor();

public:
	void SetPickUpActor(APickUpActor* PickUpActor);

	void HandlePickUp();

protected:
	/** */
	UPROPERTY(ReplicatedUsing = OnRep_Weapon, BlueprintReadOnly, Category = Equip)
	AActor* CurrentWeapon;

	UPROPERTY(Replicated)
	FEquipInfo EquipInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float LightAttackCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float HeavyAttackCost;
	
	/** RefNotify */
	UFUNCTION()
	void OnRep_Weapon();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Equip)
	void OnUpdateWeapon();

public:
	void EquipWeapon(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void LightAttack();

	UFUNCTION(BlueprintCallable)
	void HeavyAttack();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayHitSound();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	FPlayerStat PlayerStat;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void AddStaminaValue(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath();
	
	/** True : ����, False : ������� */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = Stat)
	bool bDead;
	
	/** RefNotify */
	UFUNCTION()
	void OnRep_Dead();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateDeath();
	
public:	
	FORCEINLINE bool IsDead() { return bDead; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float BlockCost;

	/** True : ����, False : �Ϲ�  */
	UPROPERTY(ReplicatedUsing = OnRep_Blocking, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bBlocking;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Blocking();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBlock(bool bBlock);

	/** ���ݹ���� �������� �ʹ� Ʋ���������� �ȵ�. */
	UFUNCTION()
	bool CheckBlockDirection(AActor* Enemy);

	/** ���� ���⿡ ���� �� ��� */
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayBlockEffect();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateBlock();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AActor* Target;

	/** True : Ÿ������, False : �Ϲ�  */
	//UPROPERTY(ReplicatedUsing = OnRep_Blocking, BlueprintReadOnly, Category = Combat)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bTargeting;

	/** True : ī�޶� ���, False, ī�޶� ��� ���� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bLockCamera;

public:
	void SetLockCamera(AActor* InTarget, bool bLock);

protected:
	/**  */
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastSpawnSouls();

public:
	/** Soul ���� */
	UFUNCTION()
	void SpawnSouls(int32 SoulsValue);

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns InteractSphere subobject **/

	/** SoulPC ���� �� UMG �ʱ� ���� **/
	UFUNCTION(BlueprintCallable)
	void SetSoulPlayerController(ASoulPlayerController* InSoulPC);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};