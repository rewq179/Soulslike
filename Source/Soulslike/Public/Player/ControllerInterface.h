// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULSLIKE_API IControllerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ShowEquipmentWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ShowInventoryWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ShowStatusWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
	void ShowOptionWidget();
};