// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "Player/PlayerAnimInstance.h"

#include "Player/ActorComponent/TargetComponent.h"
#include "Player/ActorComponent/StatComponent.h"
#include "Player/ActorComponent/InteractComponent.h"
#include "Player/ActorComponent/InventoryComponent.h"
#include "Player/ActorComponent/EquipmentComponent.h"
#include "Player/ActorComponent/ComboComponent.h"

#include "System/SoulFunctionLibrary.h"

#include "Enemy/Enemy.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Engine.h"

#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ASoulCharacter::ASoulCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Capusle :: 카메라 Ignore
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 각종 Value 설정
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	MoveSpeed = 500.f;

	RollCost = 15.f;
	BlockCost = 9.f;
	LightAttackCost = 8.f;
	LightAttackRadius = 170.f;
	HeavyAttackCost = 13.f;
	HeavyAttackRadius = 240.f;

	// 카메라 붐 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; 

	// 팔로우 카메라 설정
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	// 카메라 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 무브먼트 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...   
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// 액터 컴포넌트 설정
	TargetComponent = CreateDefaultSubobject<UTargetComponent>("TargetComponent");
	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");
	InteractComponent = CreateDefaultSubobject<UInteractComponent>("InteractComponent");
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>("EquipmentComponent");
	ComboComponent = CreateDefaultSubobject<UComboComponent>("ComboComponent");

	// 태그 설정
	Tags.Add(FName("Player"));
}

////////////////////////////////////////////////////////////////////////////
//// 인터페이스 :: 애님 인스턴스, 액터 컴포넌트

void ASoulCharacter::SetEquip_Implementation(bool bEquip)
{
	ServerEquip(bEquip);
}

void ASoulCharacter::EndDeadAnim_Implementation()
{
	EndDead();
}

void ASoulCharacter::StartLightAttackAnim_Implementation()
{
	LightAttack();
}

void ASoulCharacter::StartHeavyAttackAnim_Implementation()
{
	HeavyAttack();
}

void ASoulCharacter::EndAttackAnim_Implementation()
{
	EndAttack();
}

void ASoulCharacter::EndBlockAnim_Implementation()
{
	EndBlock();
}

void ASoulCharacter::EndRollAnim_Implementation()
{
	EndRoll();
}

void ASoulCharacter::SetInteractDoor_Implementation(AInteractDoor* Door)
{
	if(InteractComponent)
	{
		InteractComponent->SetInteractDoor(Door);
	}
}

void ASoulCharacter::SetPickUpActor_Implementation(APickUpActor* Actor)
{
	if(InteractComponent)
	{
		InteractComponent->SetPickUpActor(Actor);
    }
}

// 스텟
UStatComponent* ASoulCharacter::GetStatComponent_Implementation()
{
	return StatComponent;
}

// 인벤토리
UInventoryComponent* ASoulCharacter::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

TArray<FItemTable> ASoulCharacter::GetInventory_Implementation()
{
	return InventoryComponent->GetInventory();
}

void ASoulCharacter::AddItem_Implementation(FItemTable Item)
{
	if(InventoryComponent)
	{
		InventoryComponent->AddItem(Item);
	}
}

// 장비
UEquipmentComponent* ASoulCharacter::GetEquipmentComponent_Implementation()
{
	return EquipmentComponent;
}

float ASoulCharacter::GetEquipWeight_Implementation()
{
	return EquipmentComponent->GetEquipWeight();
}

void ASoulCharacter::AddQuickItemAt_Implementation(FItemTable Item, int32 EquipIndex)
{
	if(EquipmentComponent)
	{
		EquipmentComponent->AddQuickItemAt(Item, EquipIndex);
	}	
}

////////////////////////////////////////////////////////////////////////////
//// 입력 및 시작 설정

void ASoulCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// 공격 및 방어
	PlayerInputComponent->BindAction<FMouseClickDelegate>("LightAttack", IE_Pressed, this, &ASoulCharacter::StartAttack, EPlayerAttack::Player_LightAttack);
	PlayerInputComponent->BindAction<FMouseClickDelegate>("HeavyAttack", IE_Pressed, this, &ASoulCharacter::StartAttack, EPlayerAttack::Player_HeavyAttack);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ASoulCharacter::StartBlock);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ASoulCharacter::EndBlock);

	// 이동
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &ASoulCharacter::StartRoll);

	// 기타
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASoulCharacter::ServerInteractActor);
	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &ASoulCharacter::StartTarget);
	PlayerInputComponent->BindAction("Potion", IE_Pressed, this, &ASoulCharacter::UseQuickPotion);

	// HUD
	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &ASoulCharacter::ShowMenuHUD);
	PlayerInputComponent->BindAction("TurnOff", IE_Pressed, this, &ASoulCharacter::TurnOffHUD);
	PlayerInputComponent->BindAction("Back", IE_Pressed, this, &ASoulCharacter::BackMenuHUD);

	// Quick 아이템 변경
	PlayerInputComponent->BindAction<FMouseWheelDelegate>("ChangeMagic", IE_Pressed, this, &ASoulCharacter::ShiftLeftEquipments, EMouseWheel::Wheel_Magic);
	PlayerInputComponent->BindAction<FMouseWheelDelegate>("ChangeWeapon", IE_Pressed, this, &ASoulCharacter::ShiftLeftEquipments, EMouseWheel::Wheel_Weapon);
	PlayerInputComponent->BindAction<FMouseWheelDelegate>("ChangeShield", IE_Pressed, this, &ASoulCharacter::ShiftLeftEquipments, EMouseWheel::Wheel_Shield);
	PlayerInputComponent->BindAction<FMouseWheelDelegate>("ChangePotion", IE_Pressed, this, &ASoulCharacter::ShiftLeftEquipments, EMouseWheel::Wheel_Potion);

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
			StatComponent->OnHpChanged.AddDynamic(this, &ASoulCharacter::OnPlayerStatHpChanged);
		}

		if (InteractComponent)
		{
			InteractComponent->Initialize();
		}

		if (InventoryComponent)
		{
			InventoryComponent->Initialize();
		}

		if(EquipmentComponent)
		{
			EquipmentComponent->Initialize();
		}

		if(ComboComponent)
		{
			ComboComponent->Initialize();
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// BindAxis 함수들

void ASoulCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && bMoveable && !bPlayingScene)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void ASoulCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && bMoveable && !bPlayingScene)
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
//// 구르기 관련 함수

void ASoulCharacter::StartRoll()
{
	if (bMoveable && !bPlayingScene && !bRolling && StatComponent->GetCurStamina() >= RollCost) 
	{
		StatComponent->ClearStaminaTimer();

		ServerRoll();
	}
}

void ASoulCharacter::EndRoll()
{
	SetRolling(false);

	StatComponent->PlayStaminaTimer();
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


void ASoulCharacter::SetRolling(bool bRoll)
{
	bRolling = bRoll;

	OnRep_Rolling();
}

void ASoulCharacter::OnRep_Rolling()
{
	if(PlayerAnimInstance)
	{
		PlayerAnimInstance->SetRoll(bRolling);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 공격 관련 함수들

void  ASoulCharacter::StartAttack(EPlayerAttack Attack)
{
	if (bMoveable && !bPlayingScene && EquipmentComponent && EquipmentComponent->IsWeaponEquip())
	{
		ServerAttack(Attack);
	}
}

void ASoulCharacter::ServerAttack_Implementation(EPlayerAttack Attack)
{
	if (bAttacking || bRolling || bDead || bBlocking || StatComponent == nullptr)
	{
		return;
	}

	
	switch (Attack)
	{
	case EPlayerAttack::Player_LightAttack:
		if (StatComponent->GetCurStamina() < LightAttackCost || ComboComponent == nullptr)
		{
			return;
		}
		
		MulticastPlayMontage(LightAttackMontages[ComboComponent->GetComboCount()], 1.f);
		StatComponent->AddStaminaValue(-LightAttackCost * ComboComponent->GetComboCost());
		ComboComponent->ClearComboTimer();

		break;

	case EPlayerAttack::Player_HeavyAttack:
		if (StatComponent->GetCurStamina() < HeavyAttackCost)
			return;

		MulticastPlayMontage(HeavyAttackMontage, 1.f);
		StatComponent->AddStaminaValue(-HeavyAttackCost);
		break;

	default:
		break;
	}

	StatComponent->ClearStaminaTimer();
	SetAttacking(true);
}

void ASoulCharacter::EndAttack()
{
	SetAttacking(false);

	if(ComboComponent)
	{
		ComboComponent->StartComboTimer();
	}

	if(StatComponent)
	{
		StatComponent->PlayStaminaTimer();
	}
}


void ASoulCharacter::SetAttacking(bool bAttack)
{
	bAttacking = bAttack;
	bMoveable = !bAttack;
}

void ASoulCharacter::LightAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(LightAttackRadius);
	}
}

void ASoulCharacter::HeavyAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(HeavyAttackRadius);
	}
}

void ASoulCharacter::CreateOverlapSphere(float AttackRadius)
{
	TArray<AActor*> OverlappedActors;

	const FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * AttackRadius;
	USoulFunctionLibrary::CreateOverlapSphere(GetWorld(), SphereLocation, AttackRadius, AEnemy::StaticClass(), GetOwner(), OverlappedActors);

	ApplyDamageToActorInOverlapSphere(OverlappedActors, EPlayerAttack::Player_HeavyAttack);
}

void ASoulCharacter::ApplyDamageToActorInOverlapSphere(TArray<AActor*>& OverlappedActors, EPlayerAttack PlayerAttack)
{
	if (OverlappedActors.Num() > 0 && EquipmentComponent)
	{
		for (auto& Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("Enemy"))
			{
				UGameplayStatics::ApplyDamage(Actor, EquipmentComponent->GetWeaponDamage(PlayerAttack), GetController(), this, DamageType);
			}
		}

		MulticastPlaySound(HitSound);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 사망

void ASoulCharacter::EndDead() const
{
	GetMesh()->bPauseAnims = true;
}

void ASoulCharacter::OnPlayerStatHpChanged(float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (StatComponent->GetCurHp() <= 0.f)
	{
		ServerDeath();
	}
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
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		bMoveable = true;
	}), StunTime, false);
}

////////////////////////////////////////////////////////////////////////////
//// 블럭

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

	if(StatComponent)
	{
		StatComponent->PlayStaminaTimer();
	}
}

void ASoulCharacter::ServerBlock_Implementation(bool bBlock)
{
	if (bBlock && StatComponent)
	{
		StatComponent->AddStaminaValue(-BlockCost);
		StatComponent->ClearStaminaTimer();
	}

	bBlocking = bBlock;
	OnRep_Blocking();
}

void ASoulCharacter::OnRep_Blocking()
{
	if(PlayerAnimInstance)
	{
		PlayerAnimInstance->SetBlock(bBlocking);
	}
}

bool ASoulCharacter::IsBlocked(AActor* Enemy) const
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
//// 액터 컴포넌트

void ASoulCharacter::ServerEquip_Implementation(bool bEquip)
{
	MulticastEquip(bEquip);
}

void ASoulCharacter::MulticastEquip_Implementation(bool bEquip)
{
	if(PlayerAnimInstance)
	{
		PlayerAnimInstance->SetEquip(bEquip);
	}
}

void ASoulCharacter::ServerInteractActor_Implementation()
{
	if(InteractComponent)
	{
		InteractComponent->InteractActor();
	}
}

void ASoulCharacter::AddSoulsValue(int32 Value)
{
	if(StatComponent == nullptr || InventoryComponent == nullptr)
	{
		return;
	}
	
	StatComponent->AddSoulsValue(Value);
	InventoryComponent->OwnerController->OnUpdateInventory();
	
	MulticastPlayParticle(SoulParticle);
}

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
//// 보스 몬스터

void ASoulCharacter::SetBossEnemy(AEnemy* InEnemy)
{
	if(SoulPC == nullptr)
	{
		return;
	}
	
	if (InEnemy == nullptr)
	{
		BossEnemy->OnEnemyHpChanged.RemoveDynamic(this, &ASoulCharacter::OnEnemyHpChanged);
		BossEnemy = nullptr;

		SoulPC->ClientShowEnemyHpBar(false);
	}

	else
	{
		bPlayingScene = true;
		BossEnemy = InEnemy;
		BossEnemy->OnEnemyHpChanged.AddDynamic(this, &ASoulCharacter::OnEnemyHpChanged);

		SoulPC->ClientShowEnemyHpBar(true);
		SoulPC->ClientUpdateBossName(BossEnemy->GetEnemyName());
		SoulPC->ClientUpdateBossHp(BossEnemy->GetCurHp(), BossEnemy->GetMaxHp());
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
//// 아이템

void ASoulCharacter::UseQuickPotion() 
{
	if (EquipmentComponent)
	{
		EquipmentComponent->UseQuickItem();
	}
}

void ASoulCharacter::ShowMenuHUD()
{
	if(InventoryComponent)
	{
		InventoryComponent->OwnerController->ClientShowMenuHUD();
	}
}

void ASoulCharacter::BackMenuHUD()
{
	if(InventoryComponent)
	{
		InventoryComponent->OwnerController->ClientBackMenuHUD();
	}
	
}

void ASoulCharacter::TurnOffHUD()
{
	if(InventoryComponent)
	{
		InventoryComponent->OwnerController->ClientTurnOffHUD();
	}
}

void ASoulCharacter::ShiftLeftEquipments(EMouseWheel MouseWheel)
{
	if(EquipmentComponent)
	{
		EquipmentComponent->ShiftLeftEquipments(MouseWheel);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 멀티캐스트 :: 공용

void ASoulCharacter::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		PlayAnimMontage(AnimMontage, PlayRate, AnimName);
	}
}

void ASoulCharacter::MulticastPlaySound_Implementation(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), 1.f);
	}
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
	DOREPLIFETIME(ASoulCharacter, bPlayingScene);
	DOREPLIFETIME(ASoulCharacter, bLockCamera);
	DOREPLIFETIME(ASoulCharacter, Target);
}