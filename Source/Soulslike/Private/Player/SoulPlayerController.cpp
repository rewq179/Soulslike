// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SoulPlayerController.h"

#include "Net/UnrealNetwork.h"

ASoulPlayerController::ASoulPlayerController()
{

}

void ASoulPlayerController::ClientShowPickUpName_Implementation(const FText& PickUpName)
{
	OnUpdateShowPickUpName(PickUpName);
}

void ASoulPlayerController::ClientUpdateHpBar_Implementation(float CurHp, float MaxHp)
{
	OnUpdateHpValue(CurHp, MaxHp);
}

void ASoulPlayerController::ClientUpdateStaminaBar_Implementation(float CurStamina, float MaxStamina)
{
	OnUpdateStaminaValue(CurStamina, MaxStamina);
}


void ASoulPlayerController::ClientUpdateSoulsCount_Implementation(int32 SoulsCount)
{
	OnUpdateSoulsCount(SoulsCount);
}


void ASoulPlayerController::ClientShowEnemyHpBar_Implementation(bool bActive)
{
	OnUpdateShowEnemyHpBar(bActive);
}

void ASoulPlayerController::ClientUpdateBossName_Implementation(const FText& Name)
{
	OnUpdateBossName(Name);
}

void ASoulPlayerController::ClientUpdateBossHp_Implementation(float CurHp, float MaxHp)
{
	OnUpdateBossHp(CurHp, MaxHp);
}

void ASoulPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
