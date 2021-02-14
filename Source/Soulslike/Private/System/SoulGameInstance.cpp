// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SoulGameInstance.h"


#include "Online.h"
#include "System/SoulSaveGame.h"

#include "OnlineSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "System/SoulGameModeBase.h"


USoulGameInstance::USoulGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/** 세션 생성을 위한 바인드 */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnStartOnlineGameComplete);

	/** 세션 찾기를 위한 바인드 */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnFindSessionsComplete);

	/** 세션 참여를 위한 바인드 */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &USoulGameInstance::OnJoinSessionComplete);
}

////////////////////////////////////////////////////////////////////////////
//// 서버 생성

void USoulGameInstance::HostGame(const FText& Name, bool bLanMode, int32 MaxPlayer)
{
	LobbyName = Name;
	MaxPlayers = MaxPlayer;

	// 유저 Id를 위한 로컬 플레이어
	const ULocalPlayer* Player = GetFirstGamePlayer();

	//OnShowLoadingScreen();
	HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, bLanMode, true, MaxPlayers);
}

bool USoulGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bLanMode, bool bPresence, int32 MaxPlayer)
{
	// 작업할 온라인 서브 시스템 가져오기
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// 사용할 모든 세션의 설정을 입력
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

			// 대리자들 SessionInterface의 핸들로 설정함
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			//완료시 델리게이트 호출(다만 성공할 필요는 없음)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}

	return false;
}

void USoulGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bSuccessful));

	// 세션 인터페이스를 얻을 수 있또록 온라인서브시스템 가져오기
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// StartSession 함수를 호출하기 위한 세션 인터페이스
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// 호출을 마쳤으니 델리게이트 핸들을 지움.
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			if (bSuccessful)
			{
				// StartSession의 델리게이트 설정
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// StartSessionComplete는 후에 호출된다.
				Sessions->StartSession(SessionName);
			}
		}
		
	}
}

void USoulGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bSuccessful));

	// 세션 인터페이스를 얻을 수 있또록 온라인서브시스템 가져오기
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// 델리게이트를 지우기 위해
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// 호출이 끝낫으니 델리게이트는 지움.
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// 성공시 원하는 Map을 연다.
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
			// 원하는 검색 설정을 입력!
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bLan;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			
			// Presence가 True면 쿼리 설정함.
			if (bPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// FinSession의 델리게이트 설정
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			
			// 작업 완료시 호출되는 델리게이트
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	
	else
	{
		// 문제가 발생하면 False로 직접 델리게이트 함수를 호출합니다.
		OnFindSessionsComplete(false);
	}
}

void USoulGameInstance::OnFindSessionsComplete(bool bSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));
		
			// 발견된 세션이 1개 이상이여야한다.
			const bool bSessionFound = SessionSearch->SearchResults.Num() > 0;
			
			if (bSessionFound)
			{
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx+1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
				}
			}

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
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			
			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}
		
	return bSuccessful;
}

void USoulGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));
    
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    
		if (Sessions.IsValid())
		{
			// 델리게이트 해제
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    
			// 첫 로컬 PlayerController를 가져와 CLientTravel을 호출한다.
			APlayerController * const PlayerController = GetFirstLocalPlayerController();
    
			// Client Travel을 사용할 때 쓰는 FString
			FString TravelURL;
    
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// 마지막으로 ClientTravel 호출
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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("DestroySession")));
	
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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bSuccessful));

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