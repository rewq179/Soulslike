// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataType.h"
#include "LobbyPlayerController.generated.h"


class ALobbyGameModeBase;
class USoulGameInstance;

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class")
	ALobbyGameModeBase* LobbyGameModeBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class")
	USoulGameInstance* SoulGameInstance;

	////////////////////////////////////////////////////////////////////////////
	//// 로비
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Profile")
	TArray<FPlayerProfile> PlayerProfiles;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Profile")
	FPlayerProfile PlayerProfile;
	
public:
	/** 플레이어의 초기 설정값 */
	UFUNCTION(Client, Reliable)
	void ClientInitPlayer();

	/** 로비 HUD의 초기 설정값 */
	UFUNCTION(Client, Reliable)
    void ClientInitLobbyMenu(const FText& LobbyName, const FText& InName, const FText& InTime, UTexture2D* InIcon);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnShowLobbyHUD(const FText& ServerName);

	UFUNCTION(Client, Reliable)
    void ClientUpdateLobbySettings(const FText& InName, const FText& InTime, UTexture2D* InIcon);

	UFUNCTION(BlueprintImplementableEvent, Category = "LobbyPlayerController")
    void OnUpdateLobbySettings(const FText& InName, const FText& InTime, UTexture2D* InIcon);
	
	/** 프로필 변경으로 인한 전체 유저 프로필 갱신 요청 */
	UFUNCTION(Server, Reliable)
	void ServerUpdatePlayerProfile(const FPlayerProfile& InProfile);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerCount(const int32 CurPlayer, const int32 MaxPlayer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnUpdatePlayerCount(const FText& CountText);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerProfiles(const TArray<FPlayerProfile>& InProfiles);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnUpdatePlayerProfiles();

	UFUNCTION(Client, Reliable)
    void ClientUpdateKickProfiles();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnUpdateKickProfiles();
	
	UFUNCTION(Client, Reliable)
	void ClientKicked();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnDestroySession();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
	void OnShowLoadingScreen();
	
	////////////////////////////////////////////////////////////////////////////
	//// 채팅
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
	FText SenderMessage;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
	FText SenderName;
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
    void ServerUpdateChatMessage(const FText& Message);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateChatMessage(const FText& Name, const FText& Message);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LobbyPlayerController")
    void OnUpdateChatMessage(const FText& Sender, const FText& Message);

	////////////////////////////////////////////////////////////////////////////
	//// 게터 및 세터

public:
	UFUNCTION(BlueprintCallable, Category = "LobbyPlayerController")
	void SetProfileLobbyStatus(const ELobbyStatus InStatus);

	FORCEINLINE TArray<FPlayerProfile> GetPlayerProfiles() const {return PlayerProfiles;}
	FORCEINLINE FPlayerProfile GetPlayerProfile() const {return PlayerProfile;}
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
