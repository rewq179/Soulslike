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
	
	/** 커스텀 플레이어 컨트롤러 */
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

	/** 블럭할 때 보여줄 파티클 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockParticle;

	/** 소울을 생성할 때 보여줄 파티클 */
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

	/** 좌측 시프트키 */
	void StartSprint();
	void EndSprint();

	/** 스페이스바 */
	void StartRoll();
	UFUNCTION(BlueprintCallable)
	void EndRoll();

	void StartAttack(EPlayerAttack Attack);

	/** V키 */
	void StartBlock();
	UFUNCTION(BlueprintCallable)
	void EndBlock();

	/** T키 입력 */
	void StartTarget();
	
	UFUNCTION(BlueprintCallable)
	void StartDead();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

protected:
	/** True : 이동가능, False : 이동 불가능 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Status)
	bool bMoveable;

	/** True : 전력질주(이동속도 800), False : 일반(이동솏도 500) */
	UPROPERTY(ReplicatedUsing = OnRep_Sprinting, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bSprinting;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool bSprint);

	/** RefNotify */
	UFUNCTION()
	void OnRep_Sprinting();

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Status)
	void OnUpdateSprinting();

public:
	void Sprint(bool bSprint);
	void SetSprinting(bool bSprint);
	
	void SetMaxWalkSpeed(bool bSprint);
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float RollCost;

	/** True : 구르기중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Rolling, VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bRolling;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Rolling();

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
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

	/** True : 공격중, False : 일반 */
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

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
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
	/** True : 죽음, False : 살아있음 */
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
	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateDeath();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cost)
	float BlockCost;

	/** True : 블럭중, False : 일반  */
	UPROPERTY(ReplicatedUsing = OnRep_Blocking, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bBlocking;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Blocking();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBlock(bool bBlock);

public:
	/** 공격방향과 블럭방향이 너무 틀어져있으면 안됨. */
	UFUNCTION()
	bool IsBlocked(AActor* Enemy);

	void PlayBlockEffect();

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Stat)
	void OnUpdateBlock();

protected:
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

public:
	/** Soul 생성 */
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

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};