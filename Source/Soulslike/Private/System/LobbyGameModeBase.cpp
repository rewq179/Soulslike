// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "System/LobbyGameModeBase.h"
#include "System/SoulFunctionLibrary.h"
#include "System/SoulGameInstance.h"
#include "Player/SoulCharacter.h"
#include "Player/LobbyPlayerController.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// PlayerStart 설정
	TArray<AActor*> SpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnActors);
	SpawnPoints = SpawnActors;

	// 서버 설정
	SoulGameInstance = USoulFunctionLibrary::GetSoulGameInstance(GetWorld());
	if(SoulGameInstance)
	{
		MaxPlayer = SoulGameInstance->MaxPlayers;
		LobbyName = SoulGameInstance->LobbyName;
	}
}

////////////////////////////////////////////////////////////////////////////
//// 오버라이딩 :: 포스트 로그인, 로그아웃

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if(NewPlayer && SoulGameInstance)
	{
		auto NewLobbyPlayer = Cast<ALobbyPlayerController>(NewPlayer);
		if(NewLobbyPlayer)
		{
			PlayerControllers.Add(NewLobbyPlayer);

			NewLobbyPlayer->ClientInitPlayer();
			NewLobbyPlayer->ClientInitLobbyMenu(LobbyName, LobbyMapName, LobbyMapTime, LobbyMapIcon);
		}
		
		RespawnPlayer(NewPlayer);
	}
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	int32 PlayerIndex = -1;
	
	for(int32 Index = 0; Index < PlayerControllers.Num(); ++Index)
	{
		if(PlayerControllers[Index] == Exiting)
		{
			PlayerIndex = Index;
			
			break;
		}
	}
	
	if(PlayerIndex != -1)
	{
		PlayerControllers.RemoveAt(PlayerIndex);
		PlayerProfiles.RemoveAt(PlayerIndex);

		UpdateAll();
	}
}

////////////////////////////////////////////////////////////////////////////
//// 서버 및 로비에서 사용됨

void ALobbyGameModeBase::UpdateLobbySettings(const FText& InName, const FText& InTime, int32 InId, UTexture2D* InIcon)
{
	LobbyMapName = InName;
	LobbyMapTime = InTime;
	LobbyMapId = InId;
	LobbyMapIcon = InIcon;
	
	for(auto& Player : PlayerControllers)
	{
		Player->ClientUpdateLobbySettings(LobbyMapName, LobbyMapTime, LobbyMapIcon);
	}
}

void ALobbyGameModeBase::UpdateAll()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("")));
	CurPlayer = PlayerControllers.Num();

	if(CurPlayer > 0)
	{
		PlayerProfiles.Empty();

		// PlayerProfiles에 각각 PC의 프로필을 넣어준다. 그리고 각 PC에 PlayerCount를 초기화한다.
		for(auto& Player : PlayerControllers)
		{
			PlayerProfiles.Add(Player->GetPlayerProfile());
			Player->ClientUpdatePlayerCount(CurPlayer, MaxPlayer);
		}
	
		// 각 PC에 모든 PRofile 들을 업데이트한다.
		for(auto& Player : PlayerControllers)
		{
			Player->ClientUpdatePlayerProfiles(PlayerProfiles);
		}
		PlayerControllers[0]->ClientUpdateKickProfiles();
	}
}

void ALobbyGameModeBase::UpdateChatAll(const FText& Sender, const FText& Message)
{
	for(auto& Player : PlayerControllers)
	{
		Player->ClientUpdateChatMessage(Sender, Message);
	}
}


void ALobbyGameModeBase::KickPlayer(const int32 PlayerId)
{
	if(PlayerId < PlayerControllers.Num())
	{
		PlayerControllers[PlayerId]->ClientKicked();
	}
}

////////////////////////////////////////////////////////////////////////////
//// 게터

bool ALobbyGameModeBase::IsStartableGame() const
{
	if(CurPlayer <= 1)
	{
		return false;
	}

	for(auto& Profile : PlayerProfiles)
	{
		if(Profile.LobbyStatus > ELobbyStatus::Lobby_Ready)
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
//// 기타 함수

void ALobbyGameModeBase::RespawnPlayer(APlayerController* NewPlayer)
{
	if(NewPlayer->GetPawn())
	{
		NewPlayer->GetPawn()->Destroy();
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if(PlayerControllers.Num() < SpawnPoints.Num())
	{
		auto Character = GetWorld()->SpawnActor<ASoulCharacter>(CharacterClass, SpawnPoints[PlayerControllers.Num() -1]->GetTransform(), SpawnParameters);
	
		NewPlayer->Possess(Character);
	}
}


void ALobbyGameModeBase::PlayGame() 
{
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), "servertravel /Game/Maps/Arena01", UGameplayStatics::GetPlayerController(GetWorld(), 0));
}


