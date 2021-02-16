// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SoulGameInstance.h"
#include "System/SoulGameModeBase.h"
#include "System/SoulSaveGame.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Net/UnrealNetwork.h"


USoulGameInstance::USoulGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnJoinSessionComplete);
}

////////////////////////////////////////////////////////////////////////////
//// 서버 생성

void USoulGameInstance::HostGame(const FText& Name, bool bLanMode, int32 MaxPlayer)
{
	LobbyName = Name;
	MaxPlayers = MaxPlayer;

	const ULocalPlayer* Player = GetFirstGamePlayer();

	//OnShowLoadingScreen();
	HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, bLanMode, true, MaxPlayers);
}

bool USoulGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bLanMode, bool bPresence, int32 MaxPlayer)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bLanMode;
			SessionSettings->bUsesPresence = bPresence;
			SessionSettings->NumPublicConnections = MaxPlayer;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);

			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}

	return false;
}

void USoulGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			if (bSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				Sessions->StartSession(SessionName);
			}
		}
		
	}
}

void USoulGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, "listen");
	}
}

////////////////////////////////////////////////////////////////////////////
//// 세션 검색

void USoulGameInstance::FindGame(const bool bLanMode)
{
	const ULocalPlayer* Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), bLanMode, true);
}

void USoulGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bLan, bool bPresence)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bLan;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			
			if (bPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	
	else
	{
		OnFindSessionsComplete(false);
	}
}

void USoulGameInstance::OnFindSessionsComplete(bool bSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		
			const bool bSessionFound = SessionSearch->SearchResults.Num() > 0;
			
			OnShowFindSession(bSessionFound);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// 서버 참여

void USoulGameInstance::JoinGame()
{
	ULocalPlayer* Player = GetFirstGamePlayer();

	OnShowLoadingScreen();
	
	FOnlineSessionSearchResult SearchResult;
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// 필터링
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];

				NewJoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, SearchResult);
				
				break;
			}
		}
	}	
}

bool USoulGameInstance::NewJoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}
		
	return bSuccessful;
}

void USoulGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			APlayerController * const PlayerController = GetFirstLocalPlayerController();
    
			FString TravelURL;
    
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// 세션 삭제

void USoulGameInstance::ExitGame(const int32 PlayerId)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			Sessions->DestroySession(GameSessionName);
		}
	}
}

bool USoulGameInstance::DestorySession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName)
{
	bool bSuccessful = false;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			bSuccessful = Sessions->DestroySession(GameSessionName);
		}
	}

	return bSuccessful;
}

void USoulGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			if (bSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "Title", true);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//// 기타 함수

void USoulGameInstance::StartTitle()
{
	if(UGameplayStatics::DoesSaveGameExist("SaveSlot", 0))
	{
		OnShowMainMenu();
	}

	else
	{
		OnShowProfileMenu();
	}

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}

USoulSaveGame* USoulGameInstance::GetSoulSaveGame()
{
	USoulSaveGame* SoulSaveGame = Cast<USoulSaveGame>(UGameplayStatics::CreateSaveGameObject(USoulSaveGame::StaticClass()));

	return SoulSaveGame ? SoulSaveGame : nullptr;
}

void USoulGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USoulGameInstance, LobbyName);
	DOREPLIFETIME(USoulGameInstance, MaxPlayers);
}