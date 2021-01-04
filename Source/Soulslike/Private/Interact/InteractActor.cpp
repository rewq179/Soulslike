// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractActor.h"
#include "Player/SoulCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AInteractActor::AInteractActor()
{
	bReplicates = true;
	bCanBeDamaged = false;
	SetReplicateMovement(true);

	// Mesh :: ��� ������ Ingore. �ٸ� ���� ����ƽ�̳� ���̳����� Block
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	RootComponent = Mesh;

	// Sphere :: ��� ������ Ignore. �ٸ� Pawn�� Overlap
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ��ũ ����
	Tags.Add(FName("Interact"));
}

// Called when the game starts or when spawned
void AInteractActor::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractActor::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractActor::OnOverlapEnd);
}

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
//// ��Ÿ

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


