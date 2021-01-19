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

	// 설정
	ComboTime = 0.5f;

	// 콤보 데미지 가중치
	ComboDamages.Add(1.f);
	ComboDamages.Add(1.15f);
	ComboDamages.Add(1.22f);
	ComboDamages.Add(1.45f);

	// 콤보 코스트 가중치
	ComboCosts.Add(1.f);
	ComboCosts.Add(1.1f);
	ComboCosts.Add(1.2f);
	ComboCosts.Add(1.4f);
	
	SetIsReplicatedByDefault(true);
}

void UComboComponent::Initialize()
{
	if (auto const Character = Cast<ASoulCharacter>(GetOwner()))
	{
		OwnerCharacter = Character;
		ComboMaxCount = OwnerCharacter->GetLightAttackLength();
		
		if (auto const Controller = Cast<ASoulPlayerController>(OwnerCharacter->GetController()))
		{
			OwnerController = Controller;
		}
	}
}

void UComboComponent::AddComboCount(const bool bReset) // ComboCount가 Max를 초과하거나, 리셋시 0이 된다.
{
	if(bReset)
	{
		ComboCount = 0;

		GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
	}

	else
	{
		ComboCount++;
	}
}

void UComboComponent::StartComboTimer()
{
	GetWorld()->GetTimerManager().SetTimer(ComboTimer, FTimerDelegate::CreateLambda([&]()
    {
		AddComboCount(true);
		
    }), ComboTime, false);
}

void UComboComponent::ClearComboTimer()
{
	AddComboCount(false);
	
	GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
}

int32 UComboComponent::GetComboCount()
{
	if(ComboCount >= ComboMaxCount)
	{
		AddComboCount(true);
	}

	return ComboCount;
}

void UComboComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UComboComponent, OwnerCharacter);
	DOREPLIFETIME(UComboComponent, OwnerController);
	DOREPLIFETIME(UComboComponent, ComboCount);
}


