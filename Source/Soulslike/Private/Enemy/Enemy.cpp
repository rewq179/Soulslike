// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyProjectile.h"

#include "Player/SoulCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// Sets default values
AEnemy::AEnemy()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh());

	// Melee Collision :: 처음엔 반응X, 모든 반응은 Ignore, Pawn만 오버랩
	MeleeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Melee"));
	MeleeCollision->SetupAttachment(GetMesh());
	MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeleeCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// LaunchPoint :: 원거리 공격이 생성되는 위치
	LaunchPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchPoint"));
	LaunchPoint->SetupAttachment(GetMesh());

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->SetWalkableFloorAngle(70.f);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 200.f;

	// Capusle :: 카메라 Ignore
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Mesh :: 모든 반응이 Ingore. 
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Tags.Add(FName("Enemy"));

	SoulsValue = 1;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	SetMovementState(EMovementState::STATE_Walk);

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "b_MF_Weapon_R_Socket");
	
	MeleeCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	MeleeCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &AEnemy::HandleTakeAnyDamage);
	}
}

void AEnemy::SetMovementState(EMovementState State)
{
	MovementState = State;

	switch (MovementState)
	{
	case EMovementState::STATE_Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		break;

	case EMovementState::STATE_Walk:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;

	case EMovementState::STATE_Run:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SpeedMultiplier;
		break;

	default:
		break;
	}
}

void AEnemy::SetTarget(AActor * InTarget)
{
	Target = InTarget; 

	SetMovementState(EMovementState::STATE_Run);
}

void AEnemy::ClearTarget()
{
	Target = nullptr;

	SetActorEnableCollision(false);
	SetMovementState(EMovementState::STATE_Walk);
}

void AEnemy::SetMeleeCollision(bool bActive)
{
	if (bActive)
	{
		MeleeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	else
	{
		MeleeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemy::PlayAggroMotion()
{
	MulticastPlayMontage(AggroMontage, 1.f);

	FTimerHandle WaitHandle;
	float WaitTime = AggroMontage->GetPlayLength();
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		OnAggroMoitionEnd.Broadcast();

	}), WaitTime, false);
}


float AEnemy::GetDamage()
{
	return AttackDamage[(int)MonsterAttack];
}

void AEnemy::StartAttack(EMonsterAttack Attack, int32 AttackNumber)
{
	if (Target == nullptr)
	{
		ClearTarget();

		return;
	}

	SetMonsterAttack(Attack);
	float WaitTime;

	switch (MonsterAttack)
	{
	case EMonsterAttack::MATK_LightAttack:
		MulticastPlayMontage(MeleeAttackMontages[AttackNumber], 1.f);
		WaitTime = MeleeAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_HeavyAttack:
		MulticastPlayMontage(HeavyAttackMontages[AttackNumber], 1.f);
		WaitTime = HeavyAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_RangeAttack:
		bRangeDelay = true;
		MulticastPlayMontage(RangeAttackMontages[AttackNumber], 1.f);
		WaitTime = RangeAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_ChargeAttack:
		bChargeDelay = true;
		MulticastPlayMontage(ChargeAttackMontages[AttackNumber], 1.f);
		WaitTime = ChargeAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;
		break;

	default:
		break;
	}

	SetAttackDelay(WaitTime);
}

void AEnemy::SetAttackDelay(float WaitTime)
{
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, FTimerDelegate::CreateLambda([&]() // 공격 모션 끝나면 BT에게 정보전달함
	{
		switch (MonsterAttack)
		{
		case EMonsterAttack::MATK_LightAttack:
			OnLightAttackEnd.Broadcast();
			break;

		case EMonsterAttack::MATK_HeavyAttack:
			OnHeavyAttackEnd.Broadcast();
			break;

		case EMonsterAttack::MATK_RangeAttack:
			OnRangeAttackEnd.Broadcast();
			break;

		case EMonsterAttack::MATK_ChargeAttack:
			OnChargeAttackEnd.Broadcast();
			break;

		default:
			break;
		}
	}), WaitTime, false);

	if (MonsterAttack > EMonsterAttack::MATK_HeavyAttack) // 원거리 공격과 차징 공격은 계속해서 사용못함.
	{
		float DelayTime;

		switch (MonsterAttack)
		{
		case EMonsterAttack::MATK_RangeAttack:
			DelayTime = 8.f;
			GetWorld()->GetTimerManager().SetTimer(RangeDelayTimer, FTimerDelegate::CreateLambda([&]()
			{
				bRangeDelay = false;
			}), DelayTime, false);
			break;

		case EMonsterAttack::MATK_ChargeAttack:
			DelayTime = 5.f;
			GetWorld()->GetTimerManager().SetTimer(ChargeDelayTimer, FTimerDelegate::CreateLambda([&]()
			{
				bChargeDelay = false;
			}), DelayTime, false);
			break;
		}
	}
}

void AEnemy::HeavyAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(300.f, GetDamage(), 300.f);
	}
}

void AEnemy::RangeAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (ProjectileClass == nullptr)
		{
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		auto Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, LaunchPoint->GetComponentTransform(), SpawnParams);
		Projectile->Damage = GetDamage();
	}
}

void AEnemy::ChargeAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(300.f, GetDamage(), 300.f);
	}
}

void AEnemy::CreateOverlapSphere(float Radius, float Damage, float Velocity)
{
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*>IgnoreTypes;
	IgnoreTypes.Add(GetOwner());

	FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * Radius;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, ClassFilter, IgnoreTypes, OverlappedActors);
	DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(Actor, Damage, GetController(), this, DamageType);

			if (auto const Player = Cast<ASoulCharacter>(Actor))
			{
				FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Actor->GetActorLocation());
				UnitVector.Z += 1.f;

				Player->LaunchCharacter(UnitVector * Velocity, false, false);
			}
		}
	}
}


void AEnemy::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.f)
	{
		return;
	}

	// Update health clamped
	CurHp -= Damage;

	if (CurHp <= 0.f)
	{
		ServerDeath(DamageCauser);
	}
}


bool AEnemy::ServerDeath_Validate(AActor* DamageCauser)
{
	return true;
}

void AEnemy::ServerDeath_Implementation(AActor* DamageCauser)
{
	if (!bDead)
	{
		bDead = true;

		MulticastPlayAnimation(DeathAnim, false);
		GetCharacterMovement()->SetAvoidanceEnabled(false);
		GetCharacterMovement()->DisableMovement();

		if (auto const EnemyAI = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAI->StopBehaviorTree();
		}

		if (auto const Player = Cast<ASoulCharacter>(DamageCauser))
		{
			Player->SpawnSouls(SoulsValue);
		}

		ClearTarget();
	}
}

bool AEnemy::MulticastPlayAnimation_Validate(UAnimationAsset* AnimAsset, bool bLooping)
{
	return true;
}

void AEnemy::MulticastPlayAnimation_Implementation(UAnimationAsset* AnimAsset, bool bLooping)
{
	GetCapsuleComponent()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	GetMesh()->PlayAnimation(AnimAsset, false);


}

bool AEnemy::MulticastPlayMontage_Validate(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
{
	return true;
}

void AEnemy::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName AnimName = NAME_None)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		PlayAnimMontage(AnimMontage, PlayRate, AnimName);
	}
}


void AEnemy::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(OtherActor, GetDamage(), GetController(), this, DamageType);
		}
	}
}

void AEnemy::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, bDead);
}