// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"

#include "SoulCharacter.generated.h"

class ASoulPlayerController;
class UTargetComponent;
class UStatComponent;
class APickUpActor;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;

UCLASS(config = Game)
class ASoulCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Ŀ���� �÷��̾� ��Ʈ�ѷ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Class, meta = (AllowPrivateAccess = "true"))
	ASoulPlayerController* SoulPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

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
	   
	float MoveSpeed;
	float SprintSpeed;
	float BaseTurnRate;
	float BaseLookUpRate;

public:
	/** Constructor */
	ASoulCharacter();

	UTargetComponent* TargetComponent;
	UStatComponent* StatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	/** ���� ����ƮŰ */
	void StartSprint();
	void EndSprint();

	/** �����̽��� */
	void StartRoll();
	UFUNCTION(BlueprintCallable)
	void EndRoll();

	void StartAttack(EPlayerAttack Attack);

	/** VŰ */
	void StartBlock();
	UFUNCTION(BlueprintCallable)
	void EndBlock();

	/** TŰ �Է� */
	void StartTarget();
	
	UFUNCTION(BlueprintCallable)
	void StartDead();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

protected:
	/** True : �̵�����, False : �̵� �Ұ��� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bMoveable;

	/** True : ��������(�̵��ӵ� 800), False : �Ϲ�(�̵����� 500) */
	UPROPERTY(ReplicatedUsing = OnRep_Sprinting, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bSprinting;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool bSprint);

	/** RefNotify */
	UFUNCTION()
	void OnRep_Sprinting();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Status)
	void OnUpdateSprinting();

public:
	void Sprint(bool bSprint);
	void SetSprinting(bool bSprint);
	
	void SetMaxWalkSpeed(bool bSprint);
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float RollCost;

	/** True : ��������, False : �Ϲ�  */
	UPROPERTY(ReplicatedUsing = OnRep_Rolling, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bRolling;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Rolling();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Status)
	void OnUpdateRolling(bool bRoll);


public:
	void SetRolling(bool bRoll);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRoll();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRoll();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EPlayerAttack PlayerAttack;

	/** True : ������, False : �Ϲ� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bAttacking;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack();
	
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

	void CreateOverlapSphere(float Radius);
	
protected:
	/** True : ����, False : ������� */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = Stat)
	bool bDead;

	UFUNCTION()
	void OnHpChanged(float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath();
	
	/** RefNotify */
	UFUNCTION()
	void OnRep_Dead();

public:
	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateDeath();
	
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

public:
	/** ���ݹ���� �������� �ʹ� Ʋ���������� �ȵ�. */
	UFUNCTION()
	bool IsBlocked(AActor* Enemy);

	void PlayBlockEffect();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateBlock();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AActor* Target;

	/** True : Ÿ������, False : �Ϲ�  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bTargeting;

	/** True : ī�޶� ���, False, ī�޶� ��� ���� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bLockCamera;

public:
	void SetLockCamera(AActor* InTarget, bool bLock);

public:
	/** Soul ���� */
	void AddSoulsValue(int32 Value);

	void HitReaction(float StunTime, FVector KnockBack, bool bKnockDown);

public:
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlaySound(USoundBase* Sound);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayParticle(UParticleSystem* Particle);

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};