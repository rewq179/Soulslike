// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/ComboComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UComboComponent::Initialize()
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


void UComboComponent::SetComboCount(bool bCombo)
{
	bComboAttackable = bCombo;
	OwnerCharacter->GetWorldTimerManager().ClearTimer(ComboTimer);
	
	if(bCombo)
	{
		ComboCount++;

		OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(ComboTimer, FTimerDelegate::CreateLambda([&]()
        {
            SetComboCount(false);
        }), 0.5f, false);
	}
	
	else
	{
		ComboCount = 0;
	}
}

void UComboComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UComboComponent, OwnerCharacter);
	DOREPLIFETIME(UComboComponent, OwnerController);
	DOREPLIFETIME(UComboComponent, ComboCount);
}


