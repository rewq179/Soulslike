// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SoulGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ASoulGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TArray<USoundBase*> FootStepSounds;

public:
	FORCEINLINE TArray<USoundBase*> GetFootStepSounds()	const {return FootStepSounds;}
};

