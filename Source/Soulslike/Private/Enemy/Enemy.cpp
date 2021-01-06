// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyProjectile.h"

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
	
	// 타겟팅 ㅇ 표시
	/*WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetObj(TEXT("Game/Blueprints/Widget/WB_Targeting"));
	if (WidgetObj.Succeeded())
	{
		WidgetComponent->SetWidgetClass(WidgetObj.Class);
	}*/

	// 태그 설정
	Tags.Add(FName("Enemy"));

	SoulsValue = 1;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	SetMovementState(EMovementState::State_Walk);

	//WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	//WidgetComponent->SetVisibility(false);
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
		break;
	}
}

void AEnemy::SetTarget(AActor * InTarget)
{
	Target = InTarget; 

	SetMovementState(EMovementState::State_Run);
}

void AEnemy::ClearTarget()
{
	Target = nullptr;

	SetActorEnableCollision(false);
	SetMovementState(EMovementState::State_Walk);
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
//// 인터페이스


void AEnemy::LightAttackAnimStart_Implementation(bool bStart)
{
	SetLightCollision(bStart);
}

void AEnemy::HeavyAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown)
{
	HeavyAttack(Radius, Height, bKnockDown);
}

void AEnemy::ChargeAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown)
{
	ChargeAttack(Radius, Height, bKnockDown);

}

void AEnemy::RangeAttackAnimStart_Implementation()
{
	RangeAttack();
}

void AEnemy::HandAttackAnimStart_Implementation(float Radius, float Height, bool bKnockDown)
{
	// 나중에 HandAttack() 함수 만들면 수정

	HeavyAttack(Radius, Height, bKnockDown);
}

void AEnemy::PlayEffect_Implementation(UParticleSystem * InParticle, USoundBase * InSound, FTransform InTransform)
{
	PlayEffectAtTransform(InParticle, InSound, InTransform);
}

void AEnemy::DeadAnimStart_Implementation()
{
	StartDead();
}

////////////////////////////////////////////////////////////////////////////
//// 행동 트리

void AEnemy::StartAggro() // 시작 애니메이션, 플레이어 멈춤, 문 닫힘
{
	MulticastPlayMontage(AggroMontage, 1.f);

	if (Target)
	{
		if (auto const Player = Cast<ASoulCharacter>(Target))
		{
			Player->SetBossEnemy(this);
		}

		FTimerHandle WaitHandle;
		float WaitTime = AggroMontage->GetPlayLength();
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			OnAggroMoitionEnd.Broadcast();

			if (auto const Player = Cast<ASoulCharacter>(Target))
			{
				Player->SetPlayingScene(false);
			}

		}), WaitTime, false);
	}
}

void AEnemy::StartAttack(EEnemyAttack Attack, int32 AttackNumber, bool bFirstAttack)
{
	if (Target == nullptr)
	{
		ClearTarget();

		return;
	}

	SetMonsterAttack(Attack);
	int32 MontageNumber;
	float DelayTime = 0.f;

	switch (EnemyAttack)
	{
	case EEnemyAttack::Enemy_LightAttack:
		MontageNumber = FMath::RandRange(0, LightAttackMontages.Num() - 1);
		MulticastPlayMontage(LightAttackMontages[MontageNumber], 1.f);
		DelayTime = LightAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EEnemyAttack::Enemy_HeavyAttack:
		MontageNumber = FMath::RandRange(0, HeavyAttackMontages.Num() - 1);
		MulticastPlayMontage(HeavyAttackMontages[MontageNumber], 1.f);
		DelayTime = HeavyAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EEnemyAttack::Enemy_ChargeAttack:
		MontageNumber = FMath::RandRange(0, ChargeAttackMontages.Num() - 1);
		MulticastPlayMontage(ChargeAttackMontages[MontageNumber], 1.f);
		DelayTime = ChargeAttackMontages[MontageNumber]->GetPlayLength() + 0.2f;
		break;

	case EEnemyAttack::Enemy_RangeAttack:
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
		AttackEnd(DelayTime);
	}
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


void AEnemy::HeavyAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappedActors;

		const FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * 150.f;

		USoulFunctionLibrary::CreateOverlapSphere(GetWorld(), SphereLocation, 150.f, ASoulCharacter::StaticClass(), GetOwner(), OverlappedActors);

		ApplyDamageToActorInOverlapSphere(OverlappedActors, Height, bKnockDown);
	}
}

void AEnemy::ChargeAttack(float Radius, float Height, bool bKnockDown)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappedActors;

		const FVector SphereLocation = GetActorLocation() + GetActorForwardVector() * 150.f;

		USoulFunctionLibrary::CreateOverlapSphere(GetWorld(), SphereLocation, 150.f, ASoulCharacter::StaticClass(), GetOwner(), OverlappedActors);

		ApplyDamageToActorInOverlapSphere(OverlappedActors, Height, bKnockDown);
	}
}

void AEnemy::ApplyDamageToActorInOverlapSphere(TArray<AActor*>& OverlappedActors, float Height, bool bKnockDown)
{
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
	OnEnemyHpChanged.Broadcast(CurHp, MaxHp);

	if (CurHp <= 0.f)
	{
		ServerDeath(DamageCauser);
	}
}

void AEnemy::StartDead()
{
	GetMesh()->bPauseAnims = true;
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
			Player->SetBossEnemy(nullptr);
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

void AEnemy::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName AnimName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		PlayAnimMontage(AnimMontage, PlayRate, AnimName);
	}
}


void AEnemy::ToggleTargetWidget(ASoulCharacter* InCharacter, bool bHide)
{
	ShowTargetWidget(InCharacter, bHide);
}

void AEnemy::ShowTargetWidget_Implementation(ASoulCharacter* InCharacter, bool bHide)
{
	
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