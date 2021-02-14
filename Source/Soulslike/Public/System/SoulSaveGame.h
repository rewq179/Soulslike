// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "DataType.h"

#include "SoulSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USoulSaveGame();
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	FPlayerProfile PlayerProfile;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "SteamGameInstance")
    void SaveProfile(const FPlayerProfile& InProfile);
	
	UFUNCTION(BlueprintCallable, Category = "SteamGameInstance")
    void LoadProfile(FPlayerProfile& OutProfile) const;
};
