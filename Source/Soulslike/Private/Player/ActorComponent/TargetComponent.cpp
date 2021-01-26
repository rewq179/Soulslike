// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/TargetComponent.h"

#include "Player/SoulPlayerController.h"
#include "Player/SoulCharacter.h"

#include "Enemy/Enemy.h"
#include "System/SoulFunctionLibrary.h"

#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"


UTargetComponent::UTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	DetectRange = 1200.f;
	
	SetIsReplicatedByDefault(true);
}

void UTargetComponent::Initialize()
{
	if (auto const Character = Cast<ASoulCharacter>(GetOwner()))
	{
		OwnerCharacter = Character;

		if (auto const Controller = Cast<ASoulPlayerController>(OwnerCharacter->GetController()))
		{
			OwnerController = Controller;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// 타게팅

void UTargetComponent::Targeting()
{
	if (!bTargeting)
	{
		bTargeting = true;

		ServerFindTarget();
	}
}

void UTargetComponent::UnTargeting()
{
	if (bTargeting)
	{
		ServerSetLock(false);
	}
}

////////////////////////////////////////////////////////////////////////////
//// 대상 찾기

void UTargetComponent::ServerFindTarget_Implementation()
{
	TArray<AActor*> OverlappedActors;

	const FVector SphereLocation = OwnerCharacter->GetActorLocation();
	USoulFunctionLibrary::CreateOverlapSphere(GetWorld(), SphereLocation, DetectRange, AEnemy::StaticClass(), GetOwner(), OverlappedActors);
	
	TargetActors.Empty();
	Target = nullptr;

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Enemy"))
		{
			if (auto const Enemy = Cast<AEnemy>(Actor))
			{
				if(Enemy->IsDead())
				{
					break;
				}
				
				TargetActors.AddUnique(Actor);
			}
		}
	}

	for (auto& Actor : TargetActors)
	{
		//* 타겟 조건 추후 수정하기

		if (Target == nullptr)
		{
			Target = Actor;

			ServerSetLock(true);

			return;
		}
	}

	Target = nullptr;
	ServerSetLock(false);
}

void UTargetComponent::ServerSetLock_Implementation(const bool bLock)
{
	if (Target == nullptr)
	{
		return;
	}

	auto const Enemy = Cast<AEnemy>(Target);
	if(Enemy == nullptr)
	{
		return;
	}
	
	if (!bLock)
	{
		Target = nullptr;
	}

	OwnerCharacter->SetLockCamera(Target, bLock);

	Enemy->ShowTargetWidget(OwnerCharacter, bLock);
	bTargeting = bLock;
}

void UTargetComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTargetComponent, OwnerCharacter);
	DOREPLIFETIME(UTargetComponent, OwnerController);
	DOREPLIFETIME(UTargetComponent, bTargeting);
}