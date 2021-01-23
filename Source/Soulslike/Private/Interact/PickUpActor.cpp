// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PickUpActor.h"


#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Player/SoulCharacter.h"
#include "UObject/ConstructorHelpers.h"

APickUpActor::APickUpActor()
{
	// Sphere 설정
	SphereComponent->InitSphereRadius(100.f);

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

void APickUpActor::Interact(ASoulCharacter* SoulCharacter)
{
	if (!bInteracted && SoulCharacter)
	{
		// bInteracted= true;
		SoulCharacter->AddItem(ItemTable);

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

	return FText::FromString(FString::Format(TEXT("[F] 상호작용 : {0}"), args));
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
