// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "Interact/PickUpActor.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Containers/EnumAsByte.h"
#include "DrawDebugHelpers.h"

#include "Engine/Engine.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Animation/AnimInstance.h"

#include "Net/UnrealNetwork.h"
ASoulCharacter::ASoulCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Capusle :: 카메라 Ignore
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	MoveSpeed = 500.f;
	SprintSpeed = 8000.f;
	bSprinting = false;

	// 스텟 초기화
	PlayerStat.MaxHp = 500.f;
	PlayerStat.CurHp = PlayerStat.MaxHp;
	PlayerStat.MaxStamina = 200.f;
	PlayerStat.CurStamina = PlayerStat.MaxStamina;


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

	Tags.Add(FName("Player"));
}

void ASoulCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &ASoulCharacter::HandleTakeAnyDamage);

		auto SoulPC = Cast<ASoulPlayerController>(GetController());
		if (SoulPC)
		{
			//SoulPC->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
			//SoulPC->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
		}
	}
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
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASoulCharacter::ServerInteractActor);

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

////////////////////////////////////////////////////////////////////////////
//// 이동

void ASoulCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bMoveable)
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
	if ((Controller != NULL) && (Value != 0.0f) && bMoveable)
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
//// 공격

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

////////////////////////////////////////////////////////////////////////////
//// 아이템 루팅

bool ASoulCharacter::ServerInteractActor_Validate()
{
	return true;
}

void ASoulCharacter::ServerInteractActor_Implementation()
{
	if (CurrentPickUpActor == nullptr)
	{
		return;
	}

	HandlePickUp();
}

void ASoulCharacter::SetPickUpActor(APickUpActor* PickUpActor)
{
	if (PickUpActor == nullptr)
	{
		CurrentPickUpActor = nullptr;
		
		auto SoulPC = Cast<ASoulPlayerController>(GetController());
		if (SoulPC)
		{
			SoulPC->ClientShowPickUpName(FText::GetEmpty());
		}

		return;
	}

	CurrentPickUpActor = PickUpActor;
	auto SoulPC = Cast<ASoulPlayerController>(GetController());
	if (SoulPC)
	{
		SoulPC->ClientShowPickUpName(CurrentPickUpActor->PickUpInfo.Name);
	}
}

void ASoulCharacter::HandlePickUp()
{
	switch (CurrentPickUpActor->GetPickUpType())
	{
	case EPickUpType::PICK_Weapon:
		if (!EquipInfo.bWeaponEquiped)
		{
			EquipWeapon(CurrentPickUpActor);
		}
		break;

	case EPickUpType::PICK_Souls:
		if (!EquipInfo.bSoulsEquiped)
		{
		}
		break;

	case EPickUpType::PICK_Armor:
		if (!EquipInfo.bArmorEquiped)
		{
		}
		break;

	default:
		break;
	}
}

void ASoulCharacter::OnRep_Weapon()
{
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");

	//if (RightHandSocket)
	{
		//RightHandSocket->AttachActor(NewItem, MainPlayer->GetMesh());
	}

}

void ASoulCharacter::EquipWeapon(AActor* Item)
{
	CurrentWeapon = Item;
	EquipInfo.bWeaponEquiped = true;

	OnRep_Weapon();
}

void ASoulCharacter::LightAttackToTarget()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappedActors;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*>IgnoreTypes;
		IgnoreTypes.Add(GetOwner());
		float Radius = 130.f;
		FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * Radius * 1.5f;

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, nullptr, IgnoreTypes, OverlappedActors);

		DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

		UE_LOG(LogTemp, Log, TEXT("Count : %d "), OverlappedActors.Num());

		for (auto& Actor : OverlappedActors)
		{
			UGameplayStatics::ApplyDamage(Actor, 350.f, GetController(), this, DamageType);
		}
	}
}

void ASoulCharacter::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.f)
	{
		return;
	}

	// Update health clamped
	PlayerStat.CurHp -= Damage;
	auto SoulPC = Cast<ASoulPlayerController>(GetController());
	if (SoulPC)
	{
		SoulPC->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
	}

	if (PlayerStat.CurHp <= 0.f)
	{
		ServerDeath();
	}
}

bool ASoulCharacter::ServerDeath_Validate()
{
	return true;
}

void ASoulCharacter::ServerDeath_Implementation()
{
	if (!bDead)
	{
		bDead = true;
		bMoveable = false;
		
		auto SoulPC = Cast<ASoulPlayerController>(GetController());
		if (SoulPC)
		{
			SoulPC->ShowDeadScreenWidget();
		}

		OnRep_Dead();
	}
}

void ASoulCharacter::OnRep_Dead()
{
	GetMesh()->PlayAnimation(DeathAnim, false);

	GetCapsuleComponent()->SetCollisionProfileName("Ragdoll");
	/*
	GetMesh()->SetCollisionProfileName("Ragdoll");

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeRigidBody();*/
}


////////////////////////////////////////////////////////////////////////////
//// 애니메이션 재생

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

void ASoulCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoulCharacter, bMoveable);
	DOREPLIFETIME(ASoulCharacter, bSprinting);
	DOREPLIFETIME(ASoulCharacter, bRolling);
	DOREPLIFETIME(ASoulCharacter, bAttacking);
	DOREPLIFETIME(ASoulCharacter, CurrentPickUpActor);
	DOREPLIFETIME(ASoulCharacter, CurrentWeapon);
	DOREPLIFETIME(ASoulCharacter, EquipInfo);
	DOREPLIFETIME(ASoulCharacter, PlayerStat);
	DOREPLIFETIME(ASoulCharacter, bDead);
}