// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControllerInterface.generated.h"

/**
* 용도: SoulPlayerController 함수를 Widget의 GetOwningPlayer에서 바로 사용하기 위함.
*/

UINTERFACE(MinimalAPI)
class UControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULSLIKE_API IControllerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerController")
	void ShowEquipmentWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerController")
	void ShowInventoryWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerController")
	void ShowStatusWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerController")
	void ShowOptionWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerController")
	void ClearItemDescription();
};
