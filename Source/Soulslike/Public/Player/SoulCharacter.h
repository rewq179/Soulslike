// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"

#include "SoulCharacter.generated.h"

class UStatComponent;
class AWeapon;
class APickUpActor;
class UAnimMontage;
class UDamageType;

UCLASS(config = Game)
class ASoulCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAnimSequence *DeathAnim;
	   
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	DECLARE_DELEGATE_OneParam(FMouseClickDelegate, EPlayerAttack);

protected:
	/** True : �̵�����, False : �̵� �Ұ��� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bMoveable;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** ���� ����ƮŰ �Է� */
	void StartSprint();

	/** ���� ����ƼŰ ���� */
	void EndSprint();

	/** �����̽��� �Է�  */
	void StartRoll();



	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

protected:
	/** True : ��������(�̵��ӵ� 800), False : �Ϲ�(�̵����� 500) */
	UPROPERTY(ReplicatedUsing = OnRep_Sprinting, BlueprintReadOnly, Category = Movement)
	bool bSprinting;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Sprinting();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void OnUpdateSprinting();

	/** True : ��������, False : �Ϲ�  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Movement)
	bool bRolling;

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void OnUpdateRolling(bool bRoll);

public:
	void Sprint(bool bSprint);
	void SetSprinting(bool bSprint);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool bSprint);

	void SetRolling(bool bRoll);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRoll(bool bRoll);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastRoll(bool bRoll);

	UFUNCTION(BlueprintCallable)
	void EndRoll();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EPlayerAttack PlayerAttack;

	/** True : ������, False : �Ϲ� */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Movement)
	bool bAttacking;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTryToActivateAbility(EPlayerAttack Attack);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
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

	/** RefNotify */
	UFUNCTION()
	void OnRep_Weapon();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Equip)
	void OnUpdateWeapon();

public:
	void EquipWeapon(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void LightAttackToTarget();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	FPlayerStat PlayerStat;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* Type, class AController* InstigatedBy, AActor* DamageCauser);

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
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns InteractSphere subobject **/

	void PlayMontage(UAnimMontage* AnimMontage, FName AnimName, float PlayRate);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, FName AnimName, float PlayRate);

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};