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

	// Light Collision :: 처음엔 반응X, 모든 반응은 Ignore, Pawn만 오버랩
	LightCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Light"));
	LightCollision->SetupAttachment(GetMesh());
	LightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LightCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ProjectilePoint :: 원거리 공격이 생성되는 위치
	ProjectilePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchPoint"));
	ProjectilePoint->SetupAttachment(GetMesh());

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

	// WeaponMesh :: 모든 반응이 Ingore. 
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	Tags.Add(FName("Enemy"));

	SoulsValue = 1;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	SetMovementState(EMovementState::STATE_Walk);

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon_R_Socket");
	
	LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &AEnemy::HandleTakeAnyDamage);
	}
}

////////////////////////////////////////////////////////////////////////////
//// Set

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

void AEnemy::SetLightCollision(bool bActive)
{
	if (bActive)
	{
		LightCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	else
	{
		LightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 행동 트리

void AEnemy::StartAggro()
{
	MulticastPlayMontage(AggroMontage, 1.f);

	FTimerHandle WaitHandle;
	float WaitTime = AggroMontage->GetPlayLength();
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		OnAggroMoitionEnd.Broadcast();

	}), WaitTime, false);
}

void AEnemy::StartAttack(EMonsterAttack Attack, int32 AttackNumber, bool bFirstAttack)
{
	if (Target == nullptr)
	{
		ClearTarget();

		return;
	}

	SetMonsterAttack(Attack);
	int32 MontageNumber;
	float DelayTime;

	switch (MonsterAttack)
	{
	case EMonsterAttack::MATK_LightAttack:
		MontageNumber = FMath::RandRange(0, LightAttackMontages.Num() - 1);
		MulticastPlayMontage(LightAttackMontages[MontageNumber], 1.f);
		DelayTime = LightAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_HeavyAttack:
		MontageNumber = FMath::RandRange(0, HeavyAttackMontages.Num() - 1);
		MulticastPlayMontage(HeavyAttackMontages[MontageNumber], 1.f);
		DelayTime = HeavyAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_ChargeAttack:
		MontageNumber = FMath::RandRange(0, ChargeAttackMontages.Num() - 1);
		MulticastPlayMontage(ChargeAttackMontages[MontageNumber], 1.f);
		DelayTime = ChargeAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EMonsterAttack::MATK_RangeAttack:
		bRangeDelay = true;
		MontageNumber = FMath::RandRange(0, RangeAttackMontages.Num() - 1);
		MulticastPlayMontage(RangeAttackMontages[MontageNumber], 1.f);
		DelayTime = RangeAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	default:
		break;
	}

	if (bFirstAttack)
	{
		MulticastPlayMontage(ChargeAttackMontages[AttackNumber], 1.f);
		DelayTime = ChargeAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;

		GetWorld()->GetTimerManager().SetTimer(AttackTimer, FTimerDelegate::CreateLambda([&]() // 공격 모션 끝나면 BT에게 정보전달함
		{
			OnFirstAttackEnd.Broadcast();

		}), DelayTime, false);
	}

	else
	{
		BrocastAttackEnd(DelayTime);
	}
}

void AEnemy::BrocastAttackEnd(float DelayTime)
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

		case EMonsterAttack::MATK_ChargeAttack:
			OnChargeAttackEnd.Broadcast();
			break;

		case EMonsterAttack::MATK_RangeAttack:
			OnRangeAttackEnd.Broadcast();

			GetWorld()->GetTimerManager().SetTimer(RangeDelayTimer, FTimerDelegate::CreateLambda([&]()
			{
				bRangeDelay = false;
			}), RangeDelayTime, false);
			break;


		default:
			break;
		}
	}), DelayTime, false);
}


void AEnemy::HeavyAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(Radius, 1.f, bKnockDown);
	}
}

void AEnemy::ChargeAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(Radius, 1.f, bKnockDown);
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

		auto Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, ProjectilePoint->GetComponentTransform(), SpawnParams);
		Projectile->Damage = GetDamage();
	}
}


void AEnemy::PlayEffectAtTransform(UParticleSystem* InParticle, USoundBase* InSound, FTransform Transform)
{
	if (InParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InParticle, Transform, true);
	}

	if (InSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InSound, Transform.GetLocation(), 1.f);
	}
}

void AEnemy::CreateOverlapSphere(float Radius, float Height, bool bKnockDown)
{
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*>IgnoreTypes;
	IgnoreTypes.Add(GetOwner());

	FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * 100;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, ASoulCharacter::StaticClass(), IgnoreTypes, OverlappedActors);
	DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(Actor, GetDamage(), GetController(), this, DamageType);

			if (auto const Player = Cast<ASoulCharacter>(Actor))
			{
				FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Actor->GetActorLocation());
				UnitVector.Z += Height;

				Player->HitReaction(1.f, UnitVector, bKnockDown);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// 스텟 변화

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

		MulticastPlayMontage(DeathMontage, 1.f);
		GetCharacterMovement()->SetAvoidanceEnabled(false);
		GetCharacterMovement()->DisableMovement();

		if (auto const EnemyAI = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAI->StopBehaviorTree();
		}

		if (auto const Player = Cast<ASoulCharacter>(DamageCauser))
		{
			Player->AddSoulsValue(SoulsValue);
		}

		ClearTarget();
	}
}

////////////////////////////////////////////////////////////////////////////
//// 기타

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
			FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), OtherActor->GetActorLocation());

			if (auto const Player = Cast<ASoulCharacter>(OtherActor))
			{
				Player->HitReaction(1.f, UnitVector, false);
			}
		}
	}
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, bDead);
}