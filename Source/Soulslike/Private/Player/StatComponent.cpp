// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/StatComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"

#include "System/SoulGameModeBase.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/Engine.h"
#include "TimerManager.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	RecoveryRate = 1.5f;
	DrainRate = -3.2f;

	// 스텟 초기화
	PlayerStat.MaxHp = 500.f;
	PlayerStat.CurHp = PlayerStat.MaxHp;
	PlayerStat.MaxStamina = 200.f;
	PlayerStat.CurStamina = PlayerStat.MaxStamina;

	SetIsReplicatedByDefault(true);
}

void UStatComponent::Initialize()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UStatComponent::HandleTakeAnyDamage);
		}
	}

	if (auto const Character = Cast<ASoulCharacter>(GetOwner()))
	{
		OwnerCharacter = Character;

		if (auto const Controller = Cast<ASoulPlayerController>(OwnerCharacter->GetController()))
		{
			OwnerController = Controller;
		}
	}

	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		OwnerController->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
		OwnerController->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
		OwnerController->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
	}), 1.f, false);
}

////////////////////////////////////////////////////////////////////////////
//// 타이머 관련

void UStatComponent::PlayStaminaTimer(bool bDrain)
{
	ClearStaminaTimers();

	if (bDrain)
	{
		OwnerCharacter->GetWorldTimerManager().SetTimer(StaminaDrainTimer, this, &UStatComponent::DrainStamina, 0.25f, true, 0.1f);
	}

	else
	{
		OwnerCharacter->GetWorldTimerManager().SetTimer(StaminaRecoveryTimer, this, &UStatComponent::RecoveryStamina, 0.25f, true, 2.f);
	}
}

void UStatComponent::ClearStaminaTimers()
{
	OwnerCharacter->GetWorldTimerManager().ClearTimer(StaminaDrainTimer);
	OwnerCharacter->GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
}

void UStatComponent::RecoveryStamina()
{
	AddStaminaValue(RecoveryRate);
}

void UStatComponent::DrainStamina()
{
	AddStaminaValue(DrainRate);
}

////////////////////////////////////////////////////////////////////////////
//// 스텟 

void UStatComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * Type, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.f || DamageCauser == nullptr || OwnerCharacter->IsRoll())
	{
		return;
	}

	if (OwnerCharacter->IsBlocked(DamageCauser)) // 블럭에 성공한다면
	{
		OwnerCharacter->PlayBlockEffect();

		return;
	}

	PlayerStat.CurHp -= Damage;
	OnHpChanged.Broadcast(Damage, Type, InstigatedBy, DamageCauser);

	if (OwnerController)
	{
		OwnerController->ClientUpdateHpBar(PlayerStat.CurHp, PlayerStat.MaxHp);
	}
}

void UStatComponent::AddStaminaValue(float Value)
{
	PlayerStat.CurStamina += Value;

	if (PlayerStat.CurStamina <= 0.f)
	{
		PlayerStat.CurStamina = 0.f;

		if (OwnerCharacter->IsSprinting())
		{
			OwnerCharacter->GetWorldTimerManager().ClearTimer(StaminaDrainTimer);

			OwnerCharacter->Sprint(false);
		}
	}

	else if (PlayerStat.CurStamina > PlayerStat.MaxStamina)
	{
		PlayerStat.CurStamina = PlayerStat.MaxStamina;

		OwnerCharacter->GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
	}

	if (OwnerController)
	{
		OwnerController->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
	}

}

void UStatComponent::AddSoulsValue(float Value)
{
	PlayerStat.SoulsCount += Value;
	if (OwnerController)
	{
		OwnerController->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
	}
}


void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatComponent, OwnerCharacter);
	DOREPLIFETIME(UStatComponent, OwnerController);
	DOREPLIFETIME(UStatComponent, PlayerStat);

}
