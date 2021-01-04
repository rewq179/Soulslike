// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InteractComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "System/SoulFunctionLibrary.h"

#include "Interact/InteractActor.h"
#include "Interact/InteractDoor.h"
#include "Interact/PickUpActor.h"

#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "Net/UnrealNetwork.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called every frame
void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckInteractActor();
}


////////////////////////////////////////////////////////////////////////////
////  초기화

void UInteractComponent::Initialize()
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
////  루팅

void UInteractComponent::InteractActor()
{
	if (CurrentDoor)
	{
		CurrentDoor->Interact();

		return;
	}

	if (CurrentPickUpActor)
	{
		HandlePickUp();
	}
}

void UInteractComponent::SetPickUpActor(APickUpActor* PickUpActor)
{
	if (OwnerController == nullptr)
	{
		return;
	}

	if (PickUpActor == nullptr)
	{
		CurrentPickUpActor = nullptr;
		OwnerController->ClientShowPickUpName(FText::GetEmpty());

		return;
	}

	CurrentPickUpActor = PickUpActor;
	OwnerController->ClientShowPickUpName(CurrentPickUpActor->PickUpInfo.Name);
}

void UInteractComponent::SetInteractDoor(AInteractDoor* DoorActor)
{
	if (DoorActor == nullptr)
	{
		CurrentDoor = nullptr;

		return;
	}

	CurrentDoor = DoorActor;
}

void UInteractComponent::HandlePickUp()
{
	FEquipInfo EquipInfo = OwnerCharacter->GetEquipInfo();

	switch (CurrentPickUpActor->GetPickUpType())
	{
	case EPickUpType::PICK_Weapon:
		if (!EquipInfo.bWeaponEquiped)
		{
			EquipWeapon(CurrentPickUpActor);
		}
		break;

	case EPickUpType::PICK_Souls:
		if (!EquipInfo.bSoulsEquiped)
		{
		}
		break;

	case EPickUpType::PICK_Armor:
		if (!EquipInfo.bArmorEquiped)
		{
		}
		break;

	default:
		break;
	}
}

void UInteractComponent::OnRep_Weapon()
{
	//CurrentWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
}

void UInteractComponent::EquipWeapon(AActor* Item)
{
	CurrentWeapon = Item;
	//EquipInfo.bWeaponEquiped = true;

	OnRep_Weapon();
}

////////////////////////////////////////////////////////////////////////////
////  상호작용 물체 찾기

void UInteractComponent::CheckInteractActor()
{
	SetInteractActor();

	if (OwnerController == nullptr)
	{
		return;
	}

	if (CurActor != LastActor) // 기존껀 Off
	{
		bActorChanged = true;

		if (LastActor)
		{
			LastActor->SetRenderCustomDepth(OwnerCharacter, false);
			LastActor = nullptr;

			OwnerController->OnShowInteractMessage(false);
		}
	}

	if (CurActor) // 현재는 On
	{
		LastActor = CurActor;

		if (bActorChanged)
		{
			CurActor->SetRenderCustomDepth(OwnerCharacter, true);
			bActorChanged = false;

			OwnerController->ClientUpdateInteractMessage(CurActor->GetInteractMessage(), CurActor->GetActorLocation());
		}
	}

	else
	{
		OwnerController->OnShowInteractMessage(false);
	}

	if (LastActor && LastActor->IsInteracted())
	{
		LastActor->SetRenderCustomDepth(OwnerCharacter, false);

		OwnerController->OnShowInteractMessage(false);
	}
}


void UInteractComponent::SetInteractActor()
{
	FHitResult OutHit;
	FVector Start = OwnerCharacter->GetFollowCamera()->GetComponentLocation();

	FRotator FollowRotate = OwnerCharacter->GetFollowCamera()->GetComponentRotation();
	float Length = OwnerCharacter->GetCameraBoom()->TargetArmLength + 200.f;

	FVector End = Start + FollowRotate.Vector() * Length;

	FCollisionQueryParams TraceParms;
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, TraceParms);

	if (OutHit.GetActor())
	{
		if (OutHit.GetActor()->ActorHasTag("Interact"))
		{
			CurActor = Cast<AInteractActor>(OutHit.GetActor());

			return;
		}
	}

	CurActor = nullptr;
}

void UInteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractComponent, OwnerCharacter);
	DOREPLIFETIME(UInteractComponent, OwnerController);

	DOREPLIFETIME(UInteractComponent, CurrentDoor);
	DOREPLIFETIME(UInteractComponent, CurrentPickUpActor);
	DOREPLIFETIME(UInteractComponent, CurrentWeapon);
}