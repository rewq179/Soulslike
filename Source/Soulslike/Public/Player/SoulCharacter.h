// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType.h"

#include "SoulCharacter.generated.h"

class UStatComponent;
class AWeapon;
class UAnimMontage;

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

	   
	/** 평상시 속도 */
	float MoveSpeed;
	
	/** 전력 질주 속도 */
	float SprintSpeed;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	float BaseLookUpRate;

public:
	/** Constructor */
	ASoulCharacter();

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 좌측 시프트키 입력 */
	void StartSprint();

	/** 좌측 시프티키 해제 */
	void EndSprint();

	/** 스페이스바 입력  */
	void StartRoll();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

protected:
	/** True : 전력질주(이동속도 800), False : 일반(이동솏도 500) */
	UPROPERTY(ReplicatedUsing = OnRep_Sprinting, BlueprintReadOnly, Category = Movement)
	bool bSprinting;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Sprinting();

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void OnUpdateSprinting();

	/** True : 구르기중, False : 일반  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Movement)
	bool bRolling;

	/** BP에서 ABP_SoulCharacter의 bool값 설정 */
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

	/** True : 공격중, False : 일반 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Movement)
	bool bAttacking;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTryToActivateAbility(EPlayerAttack Attack);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void EndAttack();

public:	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns InteractSphere subobject **/

	void PlayMontage(UAnimMontage* AnimMontage, FName AnimName, float PlayRate);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, FName AnimName, float PlayRate);

	DECLARE_DELEGATE_OneParam(FMouseClickDelegate, EPlayerAttack);
};