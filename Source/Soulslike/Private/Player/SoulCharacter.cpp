// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player/SoulCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Engine/Engine.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

#include "Animation/AnimInstance.h"

ASoulCharacter::ASoulCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	MoveSpeed = 500.f;
	SprintSpeed = 8000.f;
	bSprinting = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...   
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character   
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ASoulCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoulCharacter, bSprinting);
	DOREPLIFETIME(ASoulCharacter, bRolling);
	DOREPLIFETIME(ASoulCharacter, bAttacking);
}

void ASoulCharacter::BeginPlay()
{
	Super::BeginPlay();

}

////////////////////////////////////////////////////////////////////////////
//// Input


void ASoulCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// 좌클릭 :: 약공격, 우클릭 :: 강공격
	PlayerInputComponent->BindAction<FMouseClickDelegate>("LeftClick", IE_Released, this, &ASoulCharacter::ServerTryToActivateAbility, EPlayerAttack::PATK_LightAttack);
	PlayerInputComponent->BindAction<FMouseClickDelegate>("RightClick", IE_Released, this, &ASoulCharacter::ServerTryToActivateAbility, EPlayerAttack::PATK_HeavyAttack);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASoulCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASoulCharacter::EndSprint);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &ASoulCharacter::StartRoll);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASoulCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASoulCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

////////////////////////////////////////////////////////////////////////////
//// 달리기

void ASoulCharacter::StartSprint()
{
	Sprint(true);
}

void ASoulCharacter::EndSprint()
{
	Sprint(false);
}

void ASoulCharacter::Sprint(bool bSprint)
{
	if (HasAuthority()) 
	{
		if (bSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

			SetSprinting(true);
		}

		else
		{
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
			
			SetSprinting(false);
		}

		return;
	}

	else
	{
		ServerSprint(bSprint);

		if (bSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}

		else
		{
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
		}

		return;
	}
}

void ASoulCharacter::ServerSprint_Implementation(bool bSprint)
{
	Sprint(bSprint);
}

bool ASoulCharacter::ServerSprint_Validate(bool bSprint)
{
	return true;
}

void ASoulCharacter::SetSprinting(bool bSprint)
{
	bSprinting = bSprint;

	OnRep_Sprinting();
}

void ASoulCharacter::OnRep_Sprinting()
{
	OnUpdateSprinting();
}


////////////////////////////////////////////////////////////////////////////
//// 구르기

void ASoulCharacter::StartRoll()
{
	if (!bRolling) // 구르기 중복 불가능
	{
		ServerRoll(true);
	}
}

void ASoulCharacter::ServerRoll_Implementation(bool bRoll)
{
	SetRolling(bRoll);

	MulticastRoll(bRoll);
}

bool ASoulCharacter::ServerRoll_Validate(bool bRoll)
{
	return true;
}

void ASoulCharacter::MulticastRoll_Implementation(bool bRoll)
{
	OnUpdateRolling(bRoll);

	if (bRoll)
	{
		PlayMontage(RollMontage, "ForwardRoll", 0.8f);
	}
}


bool ASoulCharacter::MulticastRoll_Validate(bool bRoll)
{
	return true;
}

void ASoulCharacter::SetRolling(bool bRoll)
{
	bRolling = bRoll;
}

void ASoulCharacter::EndRoll()
{
	ServerRoll(false);
}

void ASoulCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASoulCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 

bool ASoulCharacter::ServerTryToActivateAbility_Validate(EPlayerAttack Attack)
{
	return true;
}

void ASoulCharacter::ServerTryToActivateAbility_Implementation(EPlayerAttack Attack)
{
	if (bAttacking || bRolling)
	{
		return;
	}

	switch (Attack)
	{
	case EPlayerAttack::PATK_LightAttack:
		bAttacking = true;
		MulticastPlayMontage(LightAttackMontage, "LightAttack", 1.f);
		break;
	case EPlayerAttack::PATK_HeavyAttack:
		break;
	case EPlayerAttack::PATK_Blocking:
		break;
	default:
		break;
	}
}


bool ASoulCharacter::EndAttack_Validate()
{
	return true;
}

void ASoulCharacter::EndAttack_Implementation()
{
	bAttacking = false;
}

void ASoulCharacter::PlayMontage(UAnimMontage* AnimMontage, FName AnimName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage, PlayRate); // 해당 몽타쥬를 n배 빠르게 재상한다.
		AnimInstance->Montage_JumpToSection(AnimName, AnimMontage);
	}
}

bool ASoulCharacter::MulticastPlayMontage_Validate(UAnimMontage * AnimMontage, FName AnimName, float PlayRate)
{
	return true;
}

void ASoulCharacter::MulticastPlayMontage_Implementation(UAnimMontage * AnimMontage, FName AnimName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage, PlayRate); // 해당 몽타쥬를 n배 빠르게 재상한다.
		AnimInstance->Montage_JumpToSection(AnimName, AnimMontage);
	}
}

