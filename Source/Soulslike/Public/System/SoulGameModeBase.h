// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/SoulCharacter.h"

#include "SoulGameModeBase.generated.h"

class ASoulPlayerController;
class AEnemy;

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ASoulGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	TArray<ASoulCharacter*> SoulCharacters;
	
	class UDataTable* DungeonDataTable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable")
	TArray<FDungeonTable> DungeonTables;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon")
	AEnemy* BossEnemy;
	
public:
	ASoulGameModeBase();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TArray<USoundBase*> FootStepSounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	TArray<ASoulPlayerController*> SoulPlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon")
	int32 CurDungeonId;
	
public:
	FORCEINLINE void SetBossEnemy(AEnemy* InEnemy) {BossEnemy = InEnemy;}
	FORCEINLINE TArray<USoundBase*> GetFootStepSounds()	const {return FootStepSounds;}

	void AddPlayerController(ASoulPlayerController* InPlayerController);

	void RespawnAllPlayer();
	void EndPlayingScene();

	void AddBossEnemy(AEnemy* InBoss);
	void RemoveBossEnemy(AEnemy* InBoss);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

