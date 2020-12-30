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

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	//bReplicateMovement = true;

	InitialLifeSpan = 3.f;
	
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

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapBegin);
	OnDestroyed.AddDynamic(this, &AEnemyProjectile::OnActorDestroyed);
}


void AEnemyProjectile::OnActorDestroyed(AActor * DestroyedActor)
{
	if (bHit)
	{
		MulticastPlayParticle();
	}
}

void AEnemyProjectile::MulticastPlayParticle_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation());
}

void AEnemyProjectile::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			bHit = true;

			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, GetOwner(), DamageType);
		
			if (auto const Player = Cast<ASoulCharacter>(OtherActor))
			{
				FVector UnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), OtherActor->GetActorLocation());
				UnitVector.Z += 1;

				Player->HitReaction(1.f, UnitVector, false);
			}

			Destroy();
		}
	}
}
