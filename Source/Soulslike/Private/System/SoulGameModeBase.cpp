// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "System/SoulGameModeBase.h"
#include "Player/SoulPlayerController.h"
#include "Player/SoulCharacter.h"


#include "TimerManager.h"
#include "Enemy/Enemy.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"


ASoulGameModeBase::ASoulGameModeBase()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/DataTable/DungeonTable"));
	if (DataTable.Succeeded())
	{
		DungeonDataTable = DataTable.Object;
	}
}

void ASoulGameModeBase::BeginPlay()
{
	if(DungeonDataTable)
	{
		FDungeonRawTable* Table = DungeonDataTable->FindRow<FDungeonRawTable>(FName(*(FString::FormatAsNumber(0))), FString(""));

		FDungeonTable DungeonTable;

		DungeonTable.Id = (*Table).Id;
		DungeonTable.Name = (*Table).Name;

		DungeonTable.SpawnPoints.Add(FVector((*Table).SpawnX_0, (*Table).SpawnY_0, (*Table).SpawnZ_0));
		DungeonTable.SpawnPoints.Add(FVector((*Table).SpawnX_1, (*Table).SpawnY_1, (*Table).SpawnZ_1));
		DungeonTable.SpawnPoints.Add(FVector((*Table).SpawnX_2, (*Table).SpawnY_2, (*Table).SpawnZ_2));
		DungeonTable.SpawnPoints.Add(FVector((*Table).SpawnX_3, (*Table).SpawnY_3, (*Table).SpawnZ_3));
		
		DungeonTables.Add(DungeonTable);
	}
}

void ASoulGameModeBase::AddPlayerController(ASoulPlayerController* InPlayerController)
{
	if(InPlayerController)
	{
		SoulPlayerControllers.Add(InPlayerController);

		if(auto const Character = Cast<ASoulCharacter>(InPlayerController->GetPawn()))
		{
			SoulCharacters.Add(Character);		
		}
	}
}

void ASoulGameModeBase::RespawnAllPlayer()
{
	// 보스 몬스터의 타겟 설정 및 어그로 모션 실행
	BossEnemy->SetTarget(SoulCharacters[0]);
	BossEnemy->StartAggro();
	BossEnemy->OnAggroMoitionEnd.AddLambda([this]() -> void { EndPlayingScene(); });

	// 플레이어의 위치 텔레포트 및 던전 HUD 출력
	for(int32 PlayerIndex = 0; PlayerIndex < SoulPlayerControllers.Num(); ++PlayerIndex)
	{
		SoulCharacters[PlayerIndex]->MulticastTeleportAtLocation(DungeonTables[CurDungeonId].SpawnPoints[PlayerIndex]);

		if(SoulPlayerControllers[PlayerIndex]->IsLocalController())
		{
			SoulPlayerControllers[PlayerIndex]->ClientShowDungeonMessage(DungeonTables[CurDungeonId].Name);
		}
	}
}

void ASoulGameModeBase::EndPlayingScene()
{
	for(int32 PlayerIndex = 0; PlayerIndex < SoulPlayerControllers.Num(); ++PlayerIndex)
	{
		SoulCharacters[PlayerIndex]->MulticastEndPlayingScene();
	}
}

void ASoulGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
