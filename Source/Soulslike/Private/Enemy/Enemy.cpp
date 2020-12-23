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

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	SetMovementState(EMovementState::STATE_Walk);

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "b_MF_Weapon_R_Socket");

	FTimerHandle WaitHandle;
	float WaitTime = 1.f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		StartRoaming();

	}), WaitTime, false);

	MeleeCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	MeleeCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFindTarget && Target)
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		SetActorRotation(Rotation);
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
	bFindTarget = true;


	SetActorTickEnabled(true);
	SetMovementState(EMovementState::STATE_Run);
	StartLocation = GetActorLocation();

	
	CheckAggro();
}

void AEnemy::ClearTarget()
{
	Target = nullptr;
	bFindTarget = false;
	SetActorEnableCollision(false);
	SetMovementState(EMovementState::STATE_Walk);
	
	StartRoaming();
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

void AEnemy::CheckAggro_Implementation()
{
	GetController()->StopMovement();
	if (Target->GetDistanceTo(this) >= 800.f)
	{
		MulticastPlayMontage(AggroMontage, 1.f, "Aggro");

		FTimerHandle WaitHandle;
		float WaitTime = AggroMontage->GetPlayLength();
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			FollowTarget();

		}), WaitTime, false); 
	}

	else
	{
		FollowTarget();
	}
}


void AEnemy::StartRoaming_Implementation()
{

}

void AEnemy::FollowTarget_Implementation()
{
}

float AEnemy::GetDamage()
{
	return AttackDamage[(int)MonsterAttack];
}

void AEnemy::Attack_Implementation()
{
	if (Target == nullptr)
	{
		ClearTarget();

		return;
	}

	if (auto const Player = Cast<ASoulCharacter>(Target))
	{
		if (Player->IsDead())
		{
			ClearTarget();

			return;
		}
		
		else
		{
			float Distance = Target->GetDistanceTo(this);
			if (Distance < MeleeDistance)
			{
				if (GetPercent(AttackPercent))
				{
					SetMonsterAttack(EMonsterAttack::MATK_LightAttack);

					int32 RandNumber = FMath::RandRange(0, 2);

					MulticastPlayMontage(MeleeAttackMontages[RandNumber], 1.f, "None");

					FTimerHandle WaitHandle;
					float WaitTime = MeleeAttackMontages[RandNumber]->GetPlayLength() + 0.5f;
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
					{
						FollowTarget();

					}), WaitTime, false);
				}

				else
				{
					SetMonsterAttack(EMonsterAttack::MATK_HeavyAttack);

					MulticastPlayMontage(HeavyAttackMontage, 1.f, "None");

					FTimerHandle WaitHandle;
					float WaitTime = HeavyAttackMontage->GetPlayLength() + 0.5f;
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
					{
						FollowTarget();

					}), WaitTime, false);
				}
			}

			
			else
			{
				if (Distance <= RangeDistance)
				{
					SetMonsterAttack(EMonsterAttack::MATK_SpecialAttack);

					MulticastPlayMontage(SpecialAttackMontage, 1.f, "None");

					FTimerHandle WaitHandle;
					float WaitTime = SpecialAttackMontage->GetPlayLength() + 0.5f;
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
					{
						FollowTarget();

					}), WaitTime, false);
				}

				else
				{
					SetMonsterAttack(EMonsterAttack::MATK_RangeAttack);

					MulticastPlayMontage(RangeAttackMontage, 1.f, "None");

					FTimerHandle WaitHandle;
					float WaitTime = RangeAttackMontage->GetPlayLength() + 0.5f;
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
					{
						FollowTarget();

					}), WaitTime, false);
				}
			}
		}
	}
}

void AEnemy::HeavyAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappedActors;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*>IgnoreTypes;
		IgnoreTypes.Add(GetOwner());

		float Radius = 300.f;
		FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * Radius;

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, ClassFilter, IgnoreTypes, OverlappedActors);
		DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

		for (auto& Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("Player"))
			{
				UGameplayStatics::ApplyDamage(Actor, GetDamage(), GetController(), this, DamageType);

				if (auto const Player = Cast<ASoulCharacter>(Actor))
				{
					FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Actor->GetActorLocation());
					UnitVector.Z += 1.f;

					Player->LaunchCharacter(UnitVector * 300.f, false, false);
				}
			}
		}
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

void AEnemy::SpecialAttack()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappedActors;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*>IgnoreTypes;
		IgnoreTypes.Add(GetOwner());

		float Radius = 300.f;
		FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * Radius;

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, Radius, ObjectTypes, ClassFilter, IgnoreTypes, OverlappedActors);
		DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);

		for (auto& Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("Player"))
			{
				UGameplayStatics::ApplyDamage(Actor, GetDamage(), GetController(), this, DamageType);

				if (auto const Player = Cast<ASoulCharacter>(Actor))
				{
					FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Actor->GetActorLocation());
					UnitVector.Z += 1.f;

					Player->LaunchCharacter(UnitVector * 300.f, false, false);
				}
			}
		}
	}
}

bool AEnemy::GetPercent(float Percent)
{
	float RandNumber = FMath::RandRange(0.f, 100.f);

	if (RandNumber < Percent)
	{
		return true;
	}

	return false; 
}

bool AEnemy::MulticastPlayMontage_Validate(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
{
	return true;
}

void AEnemy::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
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