// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractDoor.h"
#include "Player/SoulCharacter.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"

#include "Net/UnrealNetwork.h"
#include "Containers/UnrealString.h"

AInteractDoor::AInteractDoor()
{
	// Mesh : 플레이어 Block
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	// Box :: 문입장시
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 욀쪽 문 :: 폰만 Block
	DoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLeft"));
	DoorLeft->SetupAttachment(Mesh);
	DoorLeft->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorLeft->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DoorLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	DoorLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	DoorLeft->SetGenerateOverlapEvents(false);

	// 오른쪽 문 :: 폰만 Block
	DoorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLight"));
	DoorRight->SetupAttachment(Mesh);
	DoorRight->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorRight->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DoorRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	DoorRight->SetGenerateOverlapEvents(false);

	// 값 설정
	OpenTime = 0.5f;
	Name = "Door";
	InteractType = EInteractType::Interact_Door;
}

////////////////////////////////////////////////////////////////////////////
//// 오버라이드

void AInteractDoor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractDoor::OnBoxOverlapBegin);
}


void AInteractDoor::Interact(ASoulCharacter* SoulCharacter)
{
	if (!bOpened && !bInteracted && SoulCharacter)
	{
		InteractDoor(true);

		DoorLeft->SetRenderCustomDepth(false);
	}
}

void AInteractDoor::SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue)
{
	if (InPlayer && InPlayer->IsLocallyControlled())
	{
		DoorLeft->SetRenderCustomDepth(bTrue);
	}
}

FText AInteractDoor::GetInteractMessage()
{
	TArray<FStringFormatArg> args;
	args.Add(FStringFormatArg(Name));

	return FText::FromString(FString::Format(TEXT("[F] 상호작용: {0}"), args));
}

////////////////////////////////////////////////////////////////////////////
//// 문

void AInteractDoor::InteractDoor(bool bOpen)
{
	if (bOpen)
	{
		MulticastDoorMoveTo(DoorLeft, DoorLeft->GetRelativeLocation(), FRotator(0.f, 0.f, 0.f));

		bInteracted = true;
	}

	else
	{
		MulticastDoorMoveTo(DoorLeft, DoorLeft->GetRelativeLocation(), FRotator(0.f, -90.f, 0.f));
	}
}


////////////////////////////////////////////////////////////////////////////
//// 문의 회전 이동

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
		if (auto const Player = Cast<ASoulCharacter>(OtherActor))
		{
			Player->SetInteractDoor(this);
		}
	}
}

void AInteractDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (auto const Player = Cast<ASoulCharacter>(OtherActor))
		{
			Player->SetInteractDoor(nullptr);
		}
	}
}


void AInteractDoor::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		InteractDoor(false);
	}
}

void AInteractDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractDoor, bOpened);
}

