// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractActor.h"
#include "Player/SoulCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

AInteractActor::AInteractActor()
{
	bReplicates = true;
	SetCanBeDamaged(false);
	SetReplicateMovement(true);

	// Mesh :: 모든 반응이 Ingore. 다만 월드 스태틱이나 다이나믹은 Block
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	RootComponent = Mesh;

	// Sphere :: 모든 반응이 Ignore. 다만 Pawn은 Overlap
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	Tags.Add(FName("Interact"));
}

void AInteractActor::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractActor::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractActor::OnOverlapEnd);
}

////////////////////////////////////////////////////////////////////////////
//// 상속

void AInteractActor::Interact()
{

}

void AInteractActor::SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue)
{

}

FText AInteractActor::GetInteractMessage()
{
	return FText();
}


////////////////////////////////////////////////////////////////////////////
//// 기타

void AInteractActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AInteractActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AInteractActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractActor, InteractType);
}


