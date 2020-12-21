// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/StatComponent.h"
#include "Player/SoulCharacter.h"

#include "System/SoulGameModeBase.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Value ¼³Á¤
	Hp = 100.f;
	   
	SetIsReplicatedByDefault(true);
}

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatComponent, Hp);
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UStatComponent::HandleTakeAnyDamage);
		}
	}
}


void UStatComponent::OnRep_Hp(float OldHp)
{
	float Damage = Hp - OldHp;

	OnHpChanged.Broadcast(this, Hp, nullptr, nullptr, nullptr);
}


void UStatComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Update Hp clamped
	Hp -= Damage;
	UE_LOG(LogTemp, Log, TEXT("Hp Changed: %s"), *FString::SanitizeFloat(Hp));

	bIsDead = Hp <= 0.0f;

	OnHpChanged.Broadcast(this, Hp, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		//ASoulslikeGameModeBase* GameMode = Cast<ASoulslikeGameModeBase>(GetWorld()->GetAuthGameMode());

		//if (GameMode)
		{
			//GameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}


void UStatComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Hp <= 0.0f)
	{
		return;
	}


	Hp += HealAmount;
	UE_LOG(LogTemp, Log, TEXT("Hp Changed: %s (+%s)"), *FString::SanitizeFloat(Hp), *FString::SanitizeFloat(HealAmount));

	OnHpChanged.Broadcast(this, Hp, nullptr, nullptr, nullptr);
}

float UStatComponent::GetHp() const
{
	return Hp;
}

