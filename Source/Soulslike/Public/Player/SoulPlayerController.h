// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ASoulPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASoulPlayerController();

protected:

public:
	UFUNCTION(Client, Reliable)
	void ClientShowPickUpName(const FText& PickUpName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateShowPickUpName(const FText& PickUpName);

	UFUNCTION(Client, Reliable)
	void ClientUpdateHpBar(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateHpValue(float CurHp, float MaxHp);

	UFUNCTION(Client, Reliable)
	void ClientUpdateStaminaBar(float CurStamina, float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateStaminaValue(float CurStamina, float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeadScreenWidget();

	UFUNCTION(Client, Reliable)
	void ClientUpdateSoulsCount(int32 SoulsCount);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateSoulsCount(int32 SoulsCount);

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
