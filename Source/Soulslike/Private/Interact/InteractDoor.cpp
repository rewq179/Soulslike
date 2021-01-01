// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractDoor.h"
#include "Player/SoulCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"

#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

AInteractDoor::AInteractDoor()
{
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	// 욀쪽 문 :: 폰만 Block
	DoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLeft"));
	DoorLeft->SetupAttachment(GetRootComponent());
	DoorLeft->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DoorLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	DoorLeft->SetGenerateOverlapEvents(false);
	DoorLeft->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 오른쪽 문 :: 폰만 Block
	DoorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLight"));
	DoorRight->SetupAttachment(GetRootComponent());
	DoorRight->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DoorRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	DoorRight->SetGenerateOverlapEvents(false);
	DoorRight->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractType = EInteractType::INTERACT_Door;

	OpenTime = 0.5f;
}

////////////////////////////////////////////////////////////////////////////
//// 문

void AInteractDoor::InteractDoor()
{
	GetWorld()->GetTimerManager().ClearTimer(DoorTimer);
	MulticastDoorMoveTo(DoorLeft, DoorLeft->GetRelativeLocation(), FRotator(0.f, 0.f, 0.f));

	StartTimer(true);
}

void AInteractDoor::CloseDoor()
{
	GetWorld()->GetTimerManager().ClearTimer(DoorTimer);
	MulticastDoorMoveTo(DoorLeft, DoorLeft->GetRelativeLocation(), FRotator(0.f, -90.f, 0.f));

	StartTimer(false);
}

void AInteractDoor::StartTimer(bool bOpen)
{
	GetWorld()->GetTimerManager().SetTimer(DoorTimer, FTimerDelegate::CreateLambda([&]()
	{
		bOpened = bOpen;

		OnRep_Opening();

	}), OpenTime, false);
}

void AInteractDoor::OnRep_Opening()
{
	if (bOpened)
	{
		DoorLeft->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}

	else
	{
		DoorLeft->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
}

////////////////////////////////////////////////////////////////////////////
//// 문의 회전 이동

bool AInteractDoor::MulticastDoorMoveTo_Validate(UStaticMeshComponent* Door, FVector Location, FRotator Rotator)
{
	return true;
}

void AInteractDoor::MulticastDoorMoveTo_Implementation(UStaticMeshComponent* Door, FVector Location, FRotator Rotator)
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(Door, Door->GetRelativeLocation(), Rotator, false, false, OpenTime, false, EMoveComponentAction::Type::Move, LatentInfo);
}

////////////////////////////////////////////////////////////////////////////
//// 오버랩

void AInteractDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (auto const Character = Cast<ASoulCharacter>(OtherActor))
		{
			Character->SetInteractDoor(this);
		}
	}
}

void AInteractDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (auto const Character = Cast<ASoulCharacter>(OtherActor))
		{
			Character->SetInteractDoor(nullptr);
		}
	}
}


void AInteractDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractDoor, bOpened);
}

