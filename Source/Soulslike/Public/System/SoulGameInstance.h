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

	/** 세션 설정에 사용할 변수 */
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	
	/* 델리게이트 :: 세션 생성시 호출 */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* 델리게이트 :: 세션 사라질때 호출 */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** 델리게이트 :: 세션 생성 / 시작을 위해 등록된 델리게이트에 대한 핸들 */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void HostGame(const FText& Name, bool bLanMode, int32 MaxPlayer);
	
	/**
	*	게임 호스팅
	*
	*	@Param		UserID			요청한 사용자의 Id
	*	@Param		SessionName		세션의 이름
	*	@Param		bLanMode		렌 게임?	
	*	@Param		bPresence		현재 세션을 생성하는 세션
	*	@Param		MaxPlayer		세션에서 허용되는 최대 플레이어의 수
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bLanMode, bool bPresence, int32 MaxPlayer);

	/**
	*	세션 생성 요청이 완료되면 실행되는 함수
	*
	*	@param SessionName 콜백에 사용될 세션의 이름
	*	@param bSuccessful 비동기 작업이 오류 없이 완료되면 True, 오류면 False 
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bSuccessful);

	/**
	*	세션 시작 요청이 완료되면 실행되는 함수
	*
	*	@param SessionName 콜백에 사용될 세션의 이름
	*	@param bSuccessful 비동기 작업이 오류 없이 완료되면 True, 오류면 False 
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bSuccessful);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 검색 및 찾기

	/** 델리게이트 :: 세션 검색 위임 */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** 델리게이트 :: 세션 검색을 위해 등록된 핸들 */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void FindGame(const bool bLanMode);
	
	/**
	*	세션 찾기
	*
	*	@param UserId		사용자의 Id
	*	@param bLan			랜 모드인가?
	*	@param bPresence	Presence 세션을 검색?
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bLan, bool bPresence);

	/**
	*	세션 검색이 완료되면 시작함
	*
	*	@param bSuccessful 비동기 작업이 오류없으면 True, 있으면 False
	*/
	void OnFindSessionsComplete(bool bSuccessful);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 참여 

	/** 델리게이트 :: 세션 참여 완료시 */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	/** 델리게이트 :: 세션 참가를 위해 등록된 */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void JoinGame();
	
	/**
	*	검색 결과를 톻안 세셤 참여
	*
	*	@param SessionName		세션 이름
	*	@param SearchResult		세션 참여
	*
	*	@return bool			성공하면 true, 그렇지 않으면 false
	*/
	bool NewJoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**
	*	세션 참여 완료시 Delegate 시작됨
	*
	*	@param SessionName		콜백에 사용되는 세션의 이름
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	////////////////////////////////////////////////////////////////////////////
	//// 세션 식제

	/** 델리게이트 :: 삭제 완료시 */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** 델리게이트 :: 세션 파괴를 위해 등록된 */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable, Category = "Network")
    void ExitGame(const int32 PlayerId);
	
	bool DestorySession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName);
	
	/**
	*	세션 파괴 완료시 델리게이트
	*
	*	@param SessionName	콜백에 사용되는 세션의 이름
	*	@param bSuccessful	오류없으면 true, 있으면 false
	*/
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
