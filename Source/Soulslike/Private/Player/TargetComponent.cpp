// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TargetComponent.h"
#include "Player/SoulPlayerController.h"
#include "Player/SoulCharacter.h"

#include "Enemy/Enemy.h"

#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Net/UnrealNetwork.h"

#include <Runtime/Engine/Classes/Engine/Engine.h>

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

bool UTargetComponent::ServerFindTarget_Validate()
{
	return true;
}

void UTargetComponent::ServerFindTarget_Implementation()
{
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*>IgnoreTypes;
	IgnoreTypes.Add(GetOwner());
	const FVector SphereLocation = OwnerCharacter->GetActorLocation();

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, DetectRange, ObjectTypes, AEnemy::StaticClass(), IgnoreTypes, OverlappedActors);

	TargetActors.Empty();
	Target = nullptr;

	for (auto& Actor : OverlappedActors)
	{
		if (Actor->ActorHasTag("Enemy"))
		{
			if (AEnemy* const Enemy = Cast<AEnemy>(Actor))
			{
				if (!Enemy->IsDead())
				{
					TargetActors.AddUnique(Actor);
				}
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

bool UTargetComponent::ServerSetLock_Validate(bool bLock)
{
	return true;
}

void UTargetComponent::ServerSetLock_Implementation(bool bLock)
{
	if (Target == nullptr)
	{
		return;
	}

	if (!bLock)
	{
		if (auto const Enemy = Cast<AEnemy>(Target))
		{
			Enemy->ToggleTargetWidget(OwnerCharacter, true);
		}

		Target = nullptr;
	}

	OwnerCharacter->SetLockCamera(Target, bLock);

	if (auto const Enemy = Cast<AEnemy>(Target))
	{
		Enemy->ToggleTargetWidget(OwnerCharacter, false);
	}

	bTargeting = bLock;

}

void UTargetComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTargetComponent, OwnerCharacter);
	DOREPLIFETIME(UTargetComponent, OwnerController);
	DOREPLIFETIME(UTargetComponent, bTargeting);
}