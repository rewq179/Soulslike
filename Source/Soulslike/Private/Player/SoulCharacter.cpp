// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "Player/TargetComponent.h"
#include "Player/StatComponent.h"

#include "Enemy/Enemy.h"
#include "Interact/PickUpActor.h"
#include "Interact/InteractDoor.h"

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
	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");
	Tags.Add(FName("Player"));
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
	PlayerInputComponent->BindAction<FMouseClickDelegate>("LeftClick", IE_Released, this, &ASoulCharacter::StartAttack, EPlayerAttack::PATK_LightAttack);
	PlayerInputComponent->BindAction<FMouseClickDelegate>("RightClick", IE_Released, this, &ASoulCharacter::StartAttack, EPlayerAttack::PATK_HeavyAttack);
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

		if (StatComponent)
		{
			StatComponent->Initialize();
			StatComponent->OnHpChanged.AddDynamic(this, &ASoulCharacter::OnHpChanged);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// BindAxis

void ASoulCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bMoveable && !bPlayingScene)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void ASoulCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bMoveable && !bPlayingScene)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void ASoulCharacter::AddControllerYawInput(float Val)
{
	if (Controller && Controller->IsLocalPlayerController() && !bPlayingScene)
	{
		if (bLockCamera)
		{
			auto CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
			auto TargetLocation = Target->GetActorLocation();

			float PitchConst = -15.f;

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
	if (Controller && Controller->IsLocalPlayerController() && !bPlayingScene)
	{
		if (Val != 0.f && !bLockCamera)
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
	if (bMoveable && !bPlayingScene)
	{
		Sprint(true);
	}
}

void ASoulCharacter::EndSprint()
{
	Sprint(false);
}

void ASoulCharacter::Sprint(bool bSprint)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		StatComponent->PlayStaminaTimer(bSprint);

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

////////////////////////////////////////////////////////////////////////////
//// 구르기

void ASoulCharacter::StartRoll()
{
	if (bMoveable && !bPlayingScene && !bRolling && StatComponent->GetCurStamina() >= RollCost) // 구르기 중복 불가능
	{
		StatComponent->ClearStaminaTimers();

		ServerRoll();
	}
}

void ASoulCharacter::EndRoll()
{
	SetRolling(false);

	StatComponent->PlayStaminaTimer(false);
}

bool ASoulCharacter::ServerRoll_Validate()
{
	return true;
}

void ASoulCharacter::ServerRoll_Implementation()
{
	StatComponent->AddStaminaValue(-RollCost);
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

void ASoulCharacter::OnRep_Rolling()
{
	OnUpdateRolling(bRolling);
}

////////////////////////////////////////////////////////////////////////////
//// 공격

void  ASoulCharacter::StartAttack(EPlayerAttack Attack)
{
	if (bMoveable && !bPlayingScene)
	{
		PlayerAttack = Attack;

		Sprint(false);

		ServerAttack();
	}
}

bool ASoulCharacter::ServerAttack_Validate()
{
	return true;
}

void ASoulCharacter::ServerAttack_Implementation()
{
	if (bAttacking || bRolling || bDead || bBlocking)
	{
		return;
	}

	switch (PlayerAttack)
	{
	case EPlayerAttack::PATK_LightAttack:
		if (StatComponent->GetCurStamina() < LightAttackCost)
			return;
		
		StatComponent->AddStaminaValue(-LightAttackCost);
		MulticastPlayMontage(LightAttackMontages[0], 1.f);
		break;

	case EPlayerAttack::PATK_HeavyAttack:
		if (StatComponent->GetCurStamina() < HeavyAttackCost)
			return;

		StatComponent->AddStaminaValue(-HeavyAttackCost);
		MulticastPlayMontage(HeavyAttackMontage, 1.f);
		break;

	default:
		break;
	}

	StatComponent->ClearStaminaTimers();
	bAttacking = true;
	bMoveable = false;
}

void ASoulCharacter::EndAttack()
{
	bAttacking = false;
	bMoveable = true;

	StatComponent->PlayStaminaTimer(false);
}


void ASoulCharacter::LightAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(130.f);
	}
}

void ASoulCharacter::HeavyAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(130.f);
	}
}

void ASoulCharacter::CreateOverlapSphere(float Radius)
{
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*>IgnoreTypes;
	IgnoreTypes.Add(GetOwner());
	FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * Radius * 1.5f;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, nullptr, IgnoreTypes, OverlappedActors);

	DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Enemy"))
		{
			UGameplayStatics::ApplyDamage(Actor, 30.f, GetController(), this, DamageType);
		}
	}

	if (OverlappedActors.Num() > 0)
	{
		MulticastPlaySound(HitSound);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 아이템 루팅

bool ASoulCharacter::ServerInteractActor_Validate()
{
	return true;
}

void ASoulCharacter::ServerInteractActor_Implementation()
{
	if (CurrentDoor)
	{
		CurrentDoor->InteractDoor();

		return;
	}

	if (CurrentPickUpActor)
	{
		HandlePickUp();
	}
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

void ASoulCharacter::SetInteractDoor(AInteractDoor * DoorActor)
{
	if (DoorActor == nullptr)
	{
		CurrentDoor = nullptr;
	
		return;
	}

	CurrentDoor = DoorActor;
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

////////////////////////////////////////////////////////////////////////////
//// 체력 관련

void ASoulCharacter::StartDead()
{
	GetMesh()->bPauseAnims = true;
}


void ASoulCharacter::OnHpChanged(float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (StatComponent->GetCurHp() <= 0.f)
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
		
		if (SoulPC)
		{
			SoulPC->ShowDeadScreenWidget();
		}

		OnRep_Dead();
	}
}

void ASoulCharacter::OnRep_Dead()
{
	MulticastPlayMontage(DeathMontage, 1.f);
}

void ASoulCharacter::HitReaction(float StunTime, FVector KnockBack, bool bKnockDown)
{
	if (bDead || bRolling || bBlocking)
	{
		return;
	}

	bMoveable = false;
	LaunchCharacter(KnockBack * 100.f, false, false);

	if (bKnockDown)
	{
		MulticastPlayMontage(KnockDownMontage, 1.f);
	}

	else
	{
		MulticastPlayMontage(KnockBackMontage, 1.f);
	}

	FTimerHandle WaitHandle;
	float WaitTime = 2.f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		bMoveable = true;
	}), WaitTime, false);
}

////////////////////////////////////////////////////////////////////////////
//// 막기

void ASoulCharacter::StartBlock()
{
	if (bMoveable && !bPlayingScene && StatComponent->GetCurStamina() < BlockCost)
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

	StatComponent->PlayStaminaTimer(false);
}

bool ASoulCharacter::ServerBlock_Validate(bool bBlock)
{
	return true;
}

void ASoulCharacter::ServerBlock_Implementation(bool bBlock)
{
	if (bBlock)
	{
		StatComponent->AddStaminaValue(-BlockCost);
		StatComponent->ClearStaminaTimers();
	}

	bBlocking = bBlock;
	OnRep_Blocking();
}

void ASoulCharacter::OnRep_Blocking()
{
	OnUpdateBlock();
}

bool ASoulCharacter::IsBlocked(AActor* Enemy)
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

void ASoulCharacter::PlayBlockEffect()
{
	MulticastPlaySound(BlockSound);
	MulticastPlayParticle(BlockParticle);
	MulticastPlayMontage(BlockMontage, 1.f);
}

////////////////////////////////////////////////////////////////////////////
//// 보스

void ASoulCharacter::SetBossEnemy(AEnemy* InEnemy)
{
	if (InEnemy == nullptr)
	{
		BossEnemy->OnEnemyHpChanged.RemoveDynamic(this, &ASoulCharacter::OnEnemyHpChanged);
		BossEnemy = nullptr;

		if (SoulPC)
		{
			SoulPC->ClientShowEnemyHpBar(false);
		}
	}

	else
	{
		BossEnemy = InEnemy;
		bPlayingScene = true;

		BossEnemy->OnEnemyHpChanged.AddDynamic(this, &ASoulCharacter::OnEnemyHpChanged);

		if (SoulPC)
		{
			SoulPC->ClientShowEnemyHpBar(true);
			SoulPC->ClientUpdateBossName(BossEnemy->GetName());
			SoulPC->ClientUpdateBossHp(BossEnemy->GetCurHp(), BossEnemy->GetMaxHp());
		}
	}
}

void ASoulCharacter::OnEnemyHpChanged(float CurHp, float MaxHp)
{
	if (SoulPC)
	{
		SoulPC->ClientUpdateBossHp(CurHp, MaxHp);
	}
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

////////////////////////////////////////////////////////////////////////////
//// 소울

void ASoulCharacter::AddSoulsValue(int32 Value)
{
	StatComponent->AddSoulsValue(Value);

	MulticastPlayParticle(SoulParticle);
}

////////////////////////////////////////////////////////////////////////////
//// 기타 

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

bool ASoulCharacter::MulticastPlaySound_Validate(USoundBase* Sound)
{
	return true;
}

void ASoulCharacter::MulticastPlaySound_Implementation(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), 1.f);
	}
}

bool ASoulCharacter::MulticastPlayParticle_Validate(UParticleSystem* Particle)
{
	return true;
}

void ASoulCharacter::MulticastPlayParticle_Implementation(UParticleSystem* Particle)
{
	if (Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, GetActorLocation());
	}
}

void ASoulCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoulCharacter, bDead);
	DOREPLIFETIME(ASoulCharacter, bRolling);
	DOREPLIFETIME(ASoulCharacter, bMoveable);
	DOREPLIFETIME(ASoulCharacter, bBlocking);
	DOREPLIFETIME(ASoulCharacter, bAttacking);
	DOREPLIFETIME(ASoulCharacter, bSprinting);
	DOREPLIFETIME(ASoulCharacter, bPlayingScene);
	DOREPLIFETIME(ASoulCharacter, bLockCamera);
	DOREPLIFETIME(ASoulCharacter, CurrentDoor); 
	DOREPLIFETIME(ASoulCharacter, CurrentPickUpActor);
	DOREPLIFETIME(ASoulCharacter, CurrentWeapon); 
	DOREPLIFETIME(ASoulCharacter, EquipInfo);
	DOREPLIFETIME(ASoulCharacter, Target);
}