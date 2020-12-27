// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "Player/TargetComponent.h"

#include "Interact/PickUpActor.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Containers/EnumAsByte.h"
#include "DrawDebugHelpers.h"

#include "Engine/Engine.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Animation/AnimInstance.h"
#include "TimerManager.h"

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
	SprintSpeed = 800.f;
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

	TargetComponent = CreateDefaultSubobject<UTargetComponent>("TargetComponent");

	Tags.Add(FName("Player"));
}

void ASoulCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &ASoulCharacter::HandleTakeAnyDamage);
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
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ASoulCharacter::StartBlock);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ASoulCharacter::EndBlock);
	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &ASoulCharacter::StartTarget);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASoulCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASoulCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ASoulCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASoulCharacter::AddControllerPitchInput);
}

void ASoulCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (auto const InSoulPC = Cast<ASoulPlayerController>(NewController))
	{
		SoulPC = InSoulPC;
		bMoveable = true;

		if (TargetComponent)
		{
			TargetComponent->Initialize();
		}

		FTimerHandle WaitHandle;
		float WaitTime = 1.f;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{

			SoulPC->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
			SoulPC->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
			SoulPC->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
		}), WaitTime, false);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 타이머 관련

void ASoulCharacter::PlayStaminaTimer(bool bDrain)
{
	ClearStaminaTimers();

	if (bDrain)
	{
		GetWorldTimerManager().SetTimer(StaminaDrainTimer, this, &ASoulCharacter::DrainStamina, 0.25f, true, 0.1f);
		/*GetWorldTimerManager().SetTimer(StaminaTimer, FTimerDelegate::CreateLambda([=]()
		{
			AddStaminaValue(-20.5f);
		}), 0.25f, true, 0.1f);*/
	}

	else
	{
		GetWorldTimerManager().SetTimer(StaminaRecoveryTimer, this, &ASoulCharacter::RecoveryStamina, 0.25f, true, 2.f);
	}
}

void ASoulCharacter::ClearStaminaTimers()
{
	GetWorldTimerManager().ClearTimer(StaminaDrainTimer);
	GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);

}


////////////////////////////////////////////////////////////////////////////
//// BindAxis

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

void ASoulCharacter::AddControllerYawInput(float Val)
{
	if (Controller && Controller->IsLocalPlayerController())
	{
		if (bLockCamera)
		{
			auto CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
			auto TargetLocation = Target->GetActorLocation();

			float PitchConst = 10.f;

			if (CameraLocation.Z >= TargetLocation.Z)
			{
				PitchConst *= -1;
			}

			CameraLocation.Z = 0.f;
			TargetLocation.Z = 0.f;


			auto LookRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);
			LookRotation.Pitch += PitchConst;

			GetController()->SetControlRotation(LookRotation);
		}

		else if (Val != 0.f && !bLockCamera)
		{
			APlayerController* const PC = CastChecked<APlayerController>(Controller);
			PC->AddYawInput(Val);
		}
	}
}

void ASoulCharacter::AddControllerPitchInput(float Val)
{
	if (Controller && Controller->IsLocalPlayerController())
	{
		if (bLockCamera)
		{

		}

		else if (Val != 0.f && !bLockCamera)
		{

			APlayerController* const PC = CastChecked<APlayerController>(Controller);
			PC->AddPitchInput(Val);
		}
	}
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
	if (GetLocalRole() == ROLE_Authority)
	{
		PlayStaminaTimer(bSprint);

		SetSprinting(bSprint);
		SetMaxWalkSpeed(bSprint);

		return;
	}

	else
	{
		ServerSprint(bSprint);
		SetMaxWalkSpeed(bSprint);

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

void ASoulCharacter::SetMaxWalkSpeed(bool bSprint)
{
	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}

	else
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void ASoulCharacter::RecoveryStamina()
{
	AddStaminaValue(1.5f);

	UE_LOG(LogTemp, Log, TEXT("Recovery"));
}

void ASoulCharacter::DrainStamina()
{
	AddStaminaValue(-3.2f);

	UE_LOG(LogTemp, Log, TEXT("Drain"));
}

////////////////////////////////////////////////////////////////////////////
//// 구르기

void ASoulCharacter::StartRoll()
{
	if (!bRolling && PlayerStat.CurStamina >= RollCost) // 구르기 중복 불가능
	{
		ClearStaminaTimers();

		ServerRoll();
	}
}

bool ASoulCharacter::ServerRoll_Validate()
{
	return true;
}

void ASoulCharacter::ServerRoll_Implementation()
{
	AddStaminaValue(-RollCost);
	SetRolling(true);

	MulticastRoll();
}


void ASoulCharacter::MulticastRoll_Implementation()
{
	MulticastPlayMontage(RollMontage, 0.8f);
}

bool ASoulCharacter::MulticastRoll_Validate()
{
	return true;
}

void ASoulCharacter::SetRolling(bool bRoll)
{
	bRolling = bRoll;

	OnRep_Rolling();
}

void ASoulCharacter::EndRoll()
{
	SetRolling(false);

	PlayStaminaTimer(false);
}

void ASoulCharacter::OnRep_Rolling()
{
	OnUpdateRolling(bRolling);
}

////////////////////////////////////////////////////////////////////////////
//// 공격

bool ASoulCharacter::ServerTryToActivateAbility_Validate(EPlayerAttack Attack)
{
	return true;
}

void ASoulCharacter::ServerTryToActivateAbility_Implementation(EPlayerAttack Attack)
{
	if (bAttacking || bRolling || bDead || bBlocking)
	{
		return;
	}

	switch (Attack)
	{
	case EPlayerAttack::PATK_LightAttack:
		if (PlayerStat.CurStamina < LightAttackCost)
			return;
		
		AddStaminaValue(-LightAttackCost);
		MulticastPlayMontage(LightAttackMontages[0], 1.f);
		break;

	case EPlayerAttack::PATK_HeavyAttack:
		if (PlayerStat.CurStamina < HeavyAttackCost)
			return;

		AddStaminaValue(-HeavyAttackCost);
		MulticastPlayMontage(HeavyAttackMontage, 1.f);
		break;

	case EPlayerAttack::PATK_Blocking:
		break;

	default:
		break;
	}

	ClearStaminaTimers();
	bAttacking = true;
	bMoveable = false;
}

void ASoulCharacter::EndAttack()
{
	bAttacking = false;
	bMoveable = true;

	PlayStaminaTimer(false);
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
		
		if (SoulPC)
		{
			SoulPC->ClientShowPickUpName(FText::GetEmpty());
		}

		return;
	}

	CurrentPickUpActor = PickUpActor;
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

}

void ASoulCharacter::EquipWeapon(AActor* Item)
{
	CurrentWeapon = Item;
	EquipInfo.bWeaponEquiped = true;

	OnRep_Weapon();
}

void ASoulCharacter::LightAttack()
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

		for (auto& Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("Enemy"))
			{
				UGameplayStatics::ApplyDamage(Actor, 350.f, GetController(), this, DamageType);
			}
		}

		if (OverlappedActors.Num() > 0)
		{
			MulticastPlayHitSound();
		}
	}
}


void ASoulCharacter::HeavyAttack()
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

		for (auto& Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("Enemy"))
			{
				UGameplayStatics::ApplyDamage(Actor, 350.f, GetController(), this, DamageType);
			}
		}

		if (OverlappedActors.Num() > 0)
		{
			MulticastPlayHitSound();
		}
	}
}

bool ASoulCharacter::MulticastPlayHitSound_Validate()
{
	return true;
}

void ASoulCharacter::MulticastPlayHitSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation(), 1.f);
	//UGameplayStatics::PlaySound2D(this, HitSound, 0.5f);
}

void ASoulCharacter::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.f || DamageCauser == nullptr)
	{
		return;
	}

	if (CheckBlockDirection(DamageCauser)) // 블럭에 성공한다면
	{
		MulticastPlayBlockEffect();

		return;
	}

	// Update health clamped
	PlayerStat.CurHp -= Damage;
	if (SoulPC)
	{
		SoulPC->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
	}

	if (PlayerStat.CurHp <= 0.f)
	{
		ServerDeath();
	}
}

void ASoulCharacter::AddStaminaValue(float Value)
{
	PlayerStat.CurStamina += Value;

	if (PlayerStat.CurStamina <= 0.f)
	{
		PlayerStat.CurStamina = 0.f;

		if (bSprinting)
		{
			GetWorldTimerManager().ClearTimer(StaminaDrainTimer);
		
			Sprint(false);
		}
	}

	else if (PlayerStat.CurStamina > PlayerStat.MaxStamina) 
	{
		PlayerStat.CurStamina = PlayerStat.MaxStamina;
		
		GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
	}

	if (SoulPC)
	{
		SoulPC->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
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
//// 블럭

void ASoulCharacter::StartBlock()
{
	if (PlayerStat.CurStamina < BlockCost)
	{
		return;
	}

	bMoveable = false;
	ServerBlock(true);
}

void ASoulCharacter::EndBlock()
{
	bMoveable = true;
	ServerBlock(false);

	PlayStaminaTimer(false);
}

bool ASoulCharacter::ServerBlock_Validate(bool bBlock)
{
	return true;
}


void ASoulCharacter::ServerBlock_Implementation(bool bBlock)
{
	if (bBlock)
	{
		AddStaminaValue(-BlockCost);
		ClearStaminaTimers();
	}

	bBlocking = bBlock;
	OnRep_Blocking();
}

void ASoulCharacter::OnRep_Blocking()
{
	OnUpdateBlock();
}

bool ASoulCharacter::CheckBlockDirection(AActor* Enemy)
{
	if (!bBlocking)
	{
		return false;
	}

	auto UnitDirection = UKismetMathLibrary::GetDirectionUnitVector(Enemy->GetActorLocation(), GetActorLocation());
	UnitDirection.Z = 0.f;
	UnitDirection.Normalize();

	auto ForwardVector = GetActorForwardVector();
	ForwardVector.Z = 0.f;
	ForwardVector.Normalize();

	if (FVector::DotProduct(ForwardVector, UnitDirection) <= 0.4f)
	{
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////
//// 타겟팅

void ASoulCharacter::StartTarget()
{
	if (bMoveable && TargetComponent)
	{
		if (!bTargeting)
		{
			bTargeting = true;

			TargetComponent->Targeting();
		}

		else
		{
			bTargeting = false;

			TargetComponent->UnTargeting();
		}
	}
}

bool ASoulCharacter::MulticastSpawnSouls_Validate()
{
	return true;
}

void ASoulCharacter::MulticastSpawnSouls_Implementation()
{
	if (SoulParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SoulParticle, GetActorLocation(), GetActorRotation(), true);
	}
}

void ASoulCharacter::SetLockCamera(AActor* InTarget, bool bLock)
{
	if (bLock)
	{
		Target = InTarget;

		bLockCamera = true;
	}

	else
	{
		Target = nullptr;

		bLockCamera = false;
	}
}

void ASoulCharacter::SpawnSouls(int32 SoulsValue)
{
	PlayerStat.SoulsCount += SoulsValue;
	if (SoulPC)
	{
		SoulPC->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
	}

	MulticastSpawnSouls();
}

bool ASoulCharacter::MulticastPlayBlockEffect_Validate()
{
	return true;
}

void ASoulCharacter::MulticastPlayBlockEffect_Implementation()
{
	if (BlockParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockParticle, GetActorLocation());
	}

	if (BlockSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlockSound, GetActorLocation(), 1.f);
	}

	MulticastPlayMontage(BlockMontage, 1.f);
}

////////////////////////////////////////////////////////////////////////////
//// 기타 공용 함수

void ASoulCharacter::SetSoulPlayerController(ASoulPlayerController * InSoulPC)
{
	if (SoulPC == nullptr && InSoulPC)
	{
		SoulPC = InSoulPC;
		bMoveable = true;

		FTimerHandle WaitHandle;
		float WaitTime = 0.5f;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			SoulPC->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
			SoulPC->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
			SoulPC->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
		}), WaitTime, false);
	}
}

bool ASoulCharacter::MulticastPlayMontage_Validate(UAnimMontage * AnimMontage, float PlayRate, FName AnimName)
{
	return true;
}

void ASoulCharacter::MulticastPlayMontage_Implementation(UAnimMontage * AnimMontage, float PlayRate, FName AnimName = NAME_None)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		PlayAnimMontage(AnimMontage, PlayRate, AnimName);
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
	DOREPLIFETIME(ASoulCharacter, bBlocking);
	DOREPLIFETIME(ASoulCharacter, Target);
	DOREPLIFETIME(ASoulCharacter, bLockCamera);
}