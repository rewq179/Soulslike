// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyProjectile.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/SoulCharacter.h"

#include "System/SoulFunctionLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// Sets default values
AEnemy::AEnemy()
{
	// WeaponMesh :: 모든 반응이 Ingore. 
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh());
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	// Light Collision :: 처음엔 반응X, 모든 반응은 Ignore, Pawn만 오버랩
	LightCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Light"));
	LightCollision->SetupAttachment(GetMesh());
	LightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LightCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ProjectilePoint :: 원거리 공격이 생성되는 위치
	ProjectilePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchPoint"));
	ProjectilePoint->SetupAttachment(GetMesh());

	// 카메라 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// AI 설정
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 캐릭터 무브먼트 설정
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
	
	// 태그 설정
	Tags.Add(FName("Enemy"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	SetMovementState(EMovementState::State_Walk);

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon_R_Socket");
	LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &AEnemy::HandleTakeAnyDamage);
	}
}


////////////////////////////////////////////////////////////////////////////
//// 인터페이스

void AEnemy::StartLightAttackAnim_Implementation()
{
	SetLightCollision(true);
}

void AEnemy::StartHeavyAttackAnim_Implementation(float Radius, float Height, bool bKnockDown)
{
	HeavyAttack(Radius, Height, bKnockDown);
}

void AEnemy::StartChargeAttackAnim_Implementation(float Radius, float Height, bool bKnockDown)
{
	ChargeAttack(Radius, Height, bKnockDown);
}

void AEnemy::StartRangeAttackAnim_Implementation()
{
	RangeAttack();
}

void AEnemy::StartHandAttackAnim_Implementation(float Radius, float Height, bool bKnockDown)
{
	// 나중에 HandAttack() 함수 만들면 수정

	HeavyAttack(Radius, Height, bKnockDown);
}

void AEnemy::EndLightAttackAnim_Implementation()
{
	SetLightCollision(false);
}

void AEnemy::EndAggroAnim_Implementation()
{
	EndAggro();
}

void AEnemy::EndDeadAnim_Implementation()
{
	StartDead();
}

void AEnemy::PlayFootStepSound_Implementation()
{
	if(EnemyAnimInstance)
	{
		EnemyAnimInstance->FootStep();
	}
}

// 이펙트 재생
void AEnemy::PlayEffect_Implementation(UParticleSystem* InParticle, USoundBase* InSound, const FTransform InTransform)
{
	PlayEffectAtTransform(InParticle, InSound, InTransform);
}

////////////////////////////////////////////////////////////////////////////
//// Set 또는 Clear

void AEnemy::SetMovementState(const EMovementState State)
{
	MovementState = State;

	switch (MovementState) // 상태에따라 이동속도를 달리한다.
	{
	case EMovementState::State_Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		break;

	case EMovementState::State_Walk:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;

	case EMovementState::State_Run:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SpeedMultiplier;
		break;

	default:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	}
}

void AEnemy::SetTarget(AActor* InTarget) // 타겟이 존재하면 이동을 빨리한다.
{
	Target = InTarget; 

	SetMovementState(EMovementState::State_Run);
}

void AEnemy::ClearTarget() // 타겟이 없으면 일반 속도로 이동한다.
{
	Target = nullptr;

	SetActorEnableCollision(false);
	SetMovementState(EMovementState::State_Walk);
}

void AEnemy::SetLightCollision(bool bActive) const // Overlap 이벤트를 설정한다.
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
//// 행동 트리에서 실행되는 함수들

void AEnemy::StartAggro()
{
	if(Target == nullptr)
	{
		return;
	}
	
	auto const Player = Cast<ASoulCharacter>(Target);
	if (Player == nullptr)
	{
		return;
	}
	
	SetEnemyHpBarHUD(Player);
	MulticastPlayMontage(AggroMontage, 1.f);
}

void AEnemy::EndAggro()
{
	OnAggroMoitionEnd.Broadcast();
	
	auto const Player = Cast<ASoulCharacter>(Target);
	if (Player)
	{
		Player->SetPlayingScene(false);
	}
}

void AEnemy::StartAttack(EEnemyAttack Attack)
{
	if (Target == nullptr)
	{
		ClearTarget();

		return;
	}

	SetEnemyAttack(Attack);

	switch (EnemyAttack)
	{
	case EEnemyAttack::Enemy_LightAttack:
		PlayAttackMontage(LightAttackMontages);
		break;

	case EEnemyAttack::Enemy_HeavyAttack:
		PlayAttackMontage(HeavyAttackMontages);
		break;

	case EEnemyAttack::Enemy_ChargeAttack:
		PlayAttackMontage(ChargeAttackMontages);
		break;

	case EEnemyAttack::Enemy_RangeAttack:
		bRangeDelay = true;
		PlayAttackMontage(RangeAttackMontages);
		break;

	default:
		break;
	}
}

void AEnemy::StartFirstAttack(int32 AttackNumber)
{
	MulticastPlayMontage(ChargeAttackMontages[AttackNumber], 1.f);
	const float DelayTime = ChargeAttackMontages[AttackNumber]->GetPlayLength() + 0.2f;
    
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, FTimerDelegate::CreateLambda([&]() // 공격 모션 끝나면 BT에게 정보전달함
    {
        OnFirstAttackEnd.Broadcast();
    
    }), DelayTime, false);
}

void AEnemy::PlayAttackMontage(TArray<UAnimMontage*>& AttackMontages)
{
	const int32 MontageNumber = FMath::RandRange(0, AttackMontages.Num() - 1);

	MulticastPlayMontage(AttackMontages[MontageNumber], 1.f);
	const float DelayTime = AttackMontages[MontageNumber]->GetPlayLength() + 0.2f;

	AttackEnd(DelayTime);
}

void AEnemy::AttackEnd(float DelayTime)
{
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, FTimerDelegate::CreateLambda([&]() // 공격 모션 끝나면 BT에게 정보전달함
	{
		switch (EnemyAttack)
		{
		case EEnemyAttack::Enemy_LightAttack:
			OnLightAttackEnd.Broadcast();
			break;

		case EEnemyAttack::Enemy_HeavyAttack:
			OnHeavyAttackEnd.Broadcast();
			break;

		case EEnemyAttack::Enemy_ChargeAttack:
			OnChargeAttackEnd.Broadcast();
			break;

		case EEnemyAttack::Enemy_RangeAttack:
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

////////////////////////////////////////////////////////////////////////////
//// 공격: 타겟에게 데미지를 줌 (Light Attack은 OnOverlapBegin에서 실행됨)

void AEnemy::HeavyAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(Radius, Height, bKnockDown);
	}
}

void AEnemy::ChargeAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CreateOverlapSphere(Radius, Height, bKnockDown);
	}
}

void AEnemy::CreateOverlapSphere(float Radius, float Height, bool bKnockDown)
{
	TArray<AActor*> OverlappedActors;
	const FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * 150.f;

	USoulFunctionLibrary::CreateOverlapSphere(GetWorld(), SphereLocation, Radius, ASoulCharacter::StaticClass(), GetOwner(), OverlappedActors);

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Player"))
		{
			USoulFunctionLibrary::ApplyDamageToPlayer(Actor, GetDamage(), GetController(), this, DamageType, Height, bKnockDown);
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

		if(const auto Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, ProjectilePoint->GetComponentTransform(), SpawnParams))
		{
			Projectile->SetDamage(GetDamage());
		}
	}
}

void AEnemy::PlayEffectAtTransform(UParticleSystem* InParticle, USoundBase* InSound, FTransform Transform) const
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


////////////////////////////////////////////////////////////////////////////
//// Hp 변화

void AEnemy::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType* Type, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f && DamageCauser == nullptr)
	{
		return;
	}

	CurHp -= Damage;
	if(IsBossEnemy())
	{
		OnBossHpChanged.Broadcast(CurHp, MaxHp);
	}

	else
	{
		OnNormalHpChanged.Broadcast(CurHp);	
	}
	
	if (CurHp <= 0.f)
	{
		ServerDeath(DamageCauser);
	}
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
			SetEnemyHpBarHUD(Player);
			Player->AddSoulsValue(SoulsValue);
		}

		ClearTarget();
	}
}

void AEnemy::StartDead() const
{
	GetMesh()->bPauseAnims = true;
}

void AEnemy::SetEnemyHpBarHUD(ASoulCharacter* Player)
{
	if(Player == nullptr)
	{
		return;
	}

	if(IsBossEnemy())
	{
		Player->ServerAddBossEnemy(this, !IsDead());
	}

	else
	{
		Player->ServerAddNormalEnemy(this, !IsDead());
	}
}

////////////////////////////////////////////////////////////////////////////
//// 기타

void AEnemy::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		PlayAnimMontage(AnimMontage, PlayRate, AnimName);
	}
}

float AEnemy::GetDamage() const
{
	const int32 DamageIndex = static_cast<int32>(EnemyAttack);

	if(DamageIndex < AttackDamages.Num())
	{
		return AttackDamages[DamageIndex];
	}

	return 0.f;
}

void AEnemy::ShowTargetWidget_Implementation(ASoulCharacter* InCharacter, bool bActive)
{

}

void AEnemy::ShowHpBarWidget_Implementation(bool bActive)
{
	
}


void AEnemy::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			USoulFunctionLibrary::ApplyDamageToPlayer(OtherActor, GetDamage(), GetController(), this, DamageType, 0.f, false);
		}
	}
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, bDead);
}