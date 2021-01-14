// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PickUpActor.h"

#include "Components/StaticMeshComponent.h"

#include "Player/SoulCharacter.h"
#include "UObject/ConstructorHelpers.h"

APickUpActor::APickUpActor()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/DataTable/ItemTable"));
	if (DataTable.Succeeded())
	{
		ItemDataTable = DataTable.Object;
	}

	InteractType = EInteractType::Interact_PickUp;
}

void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	if(ItemDataTable)
	{
		FItemTable* Table = ItemDataTable->FindRow<FItemTable>(FName(*(FString::FormatAsNumber(PickUpInfo.PickUpId))), FString(""));

		ItemTable = *Table;
	}
}

////////////////////////////////////////////////////////////////////////////
//// 오버라이드

void APickUpActor::Interact()
{
	if (!!bInteracted)
	{
		// 아이템 주기

		Mesh->SetRenderCustomDepth(false);
	}
}

void APickUpActor::SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue)
{
	if (InPlayer && InPlayer->IsLocallyControlled())
	{
		Mesh->SetRenderCustomDepth(bTrue);
	}
}

FText APickUpActor::GetInteractMessage()
{
	TArray<FStringFormatArg> args;
	args.Add(FStringFormatArg(ItemTable.Name.ToString()));

	FString string = FString::Format(TEXT("[F] Interact : {0}"), args);
	return FText::FromString(string);
}

////////////////////////////////////////////////////////////////////////////
//// 기타

void APickUpActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		auto Player = Cast<ASoulCharacter>(OtherActor);
		if (Player)
		{
			Player->SetPickUpActor(this);
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
			Player->SetPickUpActor(nullptr);
		}
	}
}
