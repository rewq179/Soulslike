// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SoulPlayerController.h"

#include "Net/UnrealNetwork.h"

ASoulPlayerController::ASoulPlayerController()
{

}

////////////////////////////////////////////////////////////////////////////
//// �÷��̾� ü�¹� ����

void ASoulPlayerController::ClientShowPickUpName_Implementation(const FText& PickUpName)
{
	OnShowPickUpName(PickUpName);
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

////////////////////////////////////////////////////////////////////////////
//// ���� ü�¹� ����

void ASoulPlayerController::ClientShowEnemyHpBar_Implementation(bool bActive)
{
	OnShowEnemyHpBar(bActive);
}

void ASoulPlayerController::ClientUpdateBossName_Implementation(const FText& Name)
{
	OnUpdateBossName(Name);
}

void ASoulPlayerController::ClientUpdateBossHp_Implementation(float CurHp, float MaxHp)
{
	OnUpdateBossHp(CurHp, MaxHp);
}

////////////////////////////////////////////////////////////////////////////
//// ��ȣ�ۿ�

void ASoulPlayerController::ClientShowInteractMessage_Implementation(bool bActive)
{
	OnShowInteractMessage(bActive);
}

void ASoulPlayerController::ClientUpdateInteractMessage_Implementation(const FText& Name, const FVector& ActorLocation)
{
	OnShowInteractMessage(true);
	OnUpdateInteractMessage(Name, ActorLocation);
}

////////////////////////////////////////////////////////////////////////////
//// ��Ÿ

void ASoulPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
