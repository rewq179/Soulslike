// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"
#include "Player/SoulCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Net/UnrealNetwork.h"
#include "System/SoulFunctionLibrary.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	// bReplicateMovement = true;

	// 발사체의 생존 시간
	InitialLifeSpan = 3.f;

	// DefaultSceneRoot :: 루트
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DeafultSceneRoot"));;
	RootComponent = DefaultSceneRoot;

	// Sphere Component :: 모든 반응은 Ignore, Pawn과 World들만 오버랩
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 그리고 cpp파일에 ObjectType 넣는 방법을 몰라서 BP에 적용해야함.
	// 따로 Projectile 오브젝트 타입을 만들었고,
	// 기본적으로 Overlap 상태, 동일한 Projectile 끼리는 Ignore

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	Particle->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->InitialSpeed = 3500.f;
}

void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapBegin);
	OnDestroyed.AddDynamic(this, &AEnemyProjectile::OnActorDestroyed);
}

void AEnemyProjectile::MulticastPlayParticle_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation());
}

void AEnemyProjectile::OnActorDestroyed(AActor * DestroyedActor)
{
	if (bHit)
	{
		MulticastPlayParticle();
	}
}

void AEnemyProjectile::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			bHit = true;

			USoulFunctionLibrary::ApplyDamageToPlayer(OtherActor, Damage, nullptr, GetOwner(), DamageType, 0.f, false);

			Destroy();
		}
	}
}
