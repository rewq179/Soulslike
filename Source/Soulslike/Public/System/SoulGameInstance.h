// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SoulGameInstance.generated.h"

class USoulSaveGame;

/**
 * 
 */

UCLASS()
class SOULSLIKE_API USoulGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	////////////////////////////////////////////////////////////////////////////
	//// 서버 설정값
	
public:
	USoulGameInstance(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	FText LobbyName;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	int32 MaxPlayers;

	////////////////////////////////////////////////////////////////////////////
	//// 서버 생성

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void HostGame(const FText& Name, bool bLanMode, int32 MaxPlayer);
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bLanMode, bool bPresence, int32 MaxPlayer);

	virtual void OnCreateSessionComplete(FName SessionName, bool bSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bSuccessful);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 검색 및 찾기

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void FindGame(const bool bLanMode);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bLan, bool bPresence);

	void OnFindSessionsComplete(bool bSuccessful);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 참여 

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void JoinGame();
	bool NewJoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 식제

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void ExitGame(const int32 PlayerId);
	bool DestorySession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName);

	virtual void OnDestroySessionComplete(FName SessionName, bool bSuccessful);

	////////////////////////////////////////////////////////////////////////////
	//// 세이브 게임
	
public:
	UFUNCTION(BlueprintCallable, Category = "SteamGameInstance")
	void StartTitle();

	UFUNCTION(BlueprintCallable, Category = "SteamSaveGAme")
	USoulSaveGame* GetSoulSaveGame();

	////////////////////////////////////////////////////////////////////////////
	//// HUD On/Off
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
	void OnShowMainMenu();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
	void OnShowProfileMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
	void OnShowHostMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
	void OnShowServerMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
    void OnShowLoadingScreen();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SteamGameInstance")
    void OnShowFindSession(const bool bFind);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
