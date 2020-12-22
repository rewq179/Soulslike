// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PickUpActor.h"
#include "Player/SoulCharacter.h"


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
		auto Character = Cast<ASoulCharacter>(OtherActor);
		if (Character)
		{
			Character->SetPickUpActor(this);
		}
	}
}

void APickUpActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		auto Character = Cast<ASoulCharacter>(OtherActor);
		if (Character)
		{
			Character->SetPickUpActor(nullptr);
		}
	}
}
