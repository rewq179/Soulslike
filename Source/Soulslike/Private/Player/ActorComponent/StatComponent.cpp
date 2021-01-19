// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActorComponent/StatComponent.h"
#include "Player/SoulCharacter.h"
#include "Player/SoulPlayerController.h"
#include "System/SoulGameModeBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

#include "Net/UnrealNetwork.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	RecoveryRate = 9.5f;

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

void UStatComponent::PlayStaminaTimer()
{
	if(OwnerCharacter)
	{
		ClearStaminaTimer();
		
		OwnerCharacter->GetWorldTimerManager().SetTimer(StaminaRecoveryTimer, this, &UStatComponent::RecoveryStamina, 0.25f, true, 2.f);
	}
}

void UStatComponent::ClearStaminaTimer()
{
	if(OwnerCharacter)
	{
		OwnerCharacter->GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
	}
}

void UStatComponent::RecoveryStamina()
{
	AddStaminaValue(RecoveryRate);
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

void UStatComponent::AddStaminaValue(const float Value)
{
	PlayerStat.CurStamina += Value;

	if (PlayerStat.CurStamina <= 0.f)
	{
		PlayerStat.CurStamina = 0.f;
	}

	else if (PlayerStat.CurStamina > PlayerStat.MaxStamina)
	{
		PlayerStat.CurStamina = PlayerStat.MaxStamina;

		ClearStaminaTimer();
	}

	if (OwnerController)
	{
		OwnerController->ClientUpdateStaminaBar(PlayerStat.CurStamina, PlayerStat.MaxStamina);
	}

}

void UStatComponent::AddSoulsValue(const float Value)
{
	PlayerStat.SoulsCount += Value;

	if (OwnerController)
	{
		OwnerController->ClientUpdateSoulsCount(PlayerStat.SoulsCount);
	}
}


TArray<FText> UStatComponent::GetStatText() const
{
	TArray<FText> Texts;

	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Constitution)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Mentality)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Witness)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Strength)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Dexterity)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Intelligence)));
	Texts.Add(FText::FromString(FString::FromInt(PlayerStat.Luck)));

	return Texts;
	
}

FText UStatComponent::GetHealthText(const EHealthType HealthType) const
{
	TArray<FStringFormatArg> Args;

	switch (HealthType)
	{
	case EHealthType::Health_Hp:
		Args.Add(FStringFormatArg(static_cast<int32>(GetCurHp())));
		Args.Add(FStringFormatArg(static_cast<int32>(GetMaxHp())));
		break;

	case EHealthType::Health_Mp:
		// args.Add(FStringFormatArg(static_cast<int32>(GetCurMp())));
		// args.Add(FStringFormatArg(static_cast<int32>(GetMaxMp())));
		break;

	case EHealthType::Health_Stamina:
		Args.Add(FStringFormatArg(static_cast<int32>(GetCurStamina())));
		Args.Add(FStringFormatArg(static_cast<int32>(GetMaxStamina())));
		break;
		
	default: ;
	}

	return FText::FromString(FString::Format(TEXT("{0}/{1}"), Args));
}

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatComponent, OwnerCharacter);
	DOREPLIFETIME(UStatComponent, OwnerController);
	DOREPLIFETIME(UStatComponent, PlayerStat);
}
