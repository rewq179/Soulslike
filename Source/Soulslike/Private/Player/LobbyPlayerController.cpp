// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"



#include "OnlineSubsystem.h"
#include "Engine/Engine.h"
#include "System/LobbyGameModeBase.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "System/SoulFunctionLibrary.h"
#include "System/SoulSaveGame.h"

void ALobbyPlayerController::ClientInitPlayer_Implementation()
{
	if(UGameplayStatics::DoesSaveGameExist("SaveSlot", 0))
	{
		auto SoulSaveGame = Cast<USoulSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));
		
		SoulSaveGame->LoadProfile(PlayerProfile);
		SoulSaveGame->SaveProfile(PlayerProfile);
	}

	ServerUpdatePlayerProfile(PlayerProfile);
}

void ALobbyPlayerController::ClientInitLobbyMenu_Implementation(const FText& LobbyName, const FText& InName, const FText& InTime, UTexture2D* InIcon)
{
	OnShowLobbyHUD(LobbyName);

	OnUpdateLobbySettings(InName, InTime, InIcon);
}


void ALobbyPlayerController::ClientUpdateLobbySettings_Implementation(const FText& InName, const FText& InTime,	UTexture2D* InIcon)
{
	OnUpdateLobbySettings(InName, InTime, InIcon);
}

void ALobbyPlayerController::ServerUpdatePlayerProfile_Implementation(const FPlayerProfile& InProfile)
{
	PlayerProfile = InProfile;

	LobbyGameModeBase = USoulFunctionLibrary::GetLobbyGameMode(GetWorld());
	if(LobbyGameModeBase)
	{
		LobbyGameModeBase->UpdateAll();
	}
}

void ALobbyPlayerController::ClientUpdatePlayerCount_Implementation(const int32 CurPlayer, const int32 MaxPlayer)
{
	TArray<FStringFormatArg> Args;
	Args.Add(FStringFormatArg(FString::FromInt(CurPlayer)));
	Args.Add(FStringFormatArg(FString::FromInt(MaxPlayer)));

	OnUpdatePlayerCount(FText::FromString(FString::Format(TEXT("플레이어 : {0}/{1}"), Args)));
}

void ALobbyPlayerController::ClientUpdatePlayerProfiles_Implementation(const TArray<FPlayerProfile>& InProfiles)
{
	PlayerProfiles = InProfiles;

	OnUpdatePlayerProfiles();
}

void ALobbyPlayerController::ClientKicked_Implementation()
{
	OnDestroySession();
}

void ALobbyPlayerController::ServerUpdateChatMessage_Implementation(const FText& Message)
{
	SenderName = PlayerProfile.PlayerName;
	SenderMessage = Message;
	
	LobbyGameModeBase->UpdateChatAll(SenderName, SenderMessage);
}

void ALobbyPlayerController::ClientUpdateChatMessage_Implementation(const FText& Name, const FText& Message)
{
	OnUpdateChatMessage(Name, Message);
}

void ALobbyPlayerController::ClientUpdateKickProfiles_Implementation()
{
	OnUpdateKickProfiles();
}

void ALobbyPlayerController::SetProfileLobbyStatus(const ELobbyStatus InStatus)
{
	PlayerProfile.LobbyStatus = InStatus;

	ServerUpdatePlayerProfile(PlayerProfile);
}

void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerController, PlayerProfiles);
	DOREPLIFETIME(ALobbyPlayerController, PlayerProfile);
	DOREPLIFETIME(ALobbyPlayerController, SenderMessage);
	DOREPLIFETIME(ALobbyPlayerController, SenderName);
}
