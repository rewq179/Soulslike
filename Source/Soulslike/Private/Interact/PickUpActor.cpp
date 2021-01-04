// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PickUpActor.h"

#include "Player/SoulCharacter.h"
#include "Player/InteractComponent.h"

// Sets default values
APickUpActor::APickUpActor()
{

}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

}

void APickUpActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		auto Player = Cast<ASoulCharacter>(OtherActor);
		if (Player)
		{
			Player->InteractComponent->SetPickUpActor(this);
		}
	}
}

void APickUpActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		auto Player = Cast<ASoulCharacter>(OtherActor);
		if (Player)
		{
			Player->InteractComponent->SetPickUpActor(nullptr);
		}
	}
}
