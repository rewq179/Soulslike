// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataType.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.generated.h"


class USoulGameInstance;
class ASoulCharacter;
class ALobbyPlayerController;

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	/** 서버에 필요한 정보(스폰 지점, 최대 플레이어 수, 로비 이름 등등... )를 초기화한다.  */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class")
	USoulGameInstance* SoulGameInstance;

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 변수

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<ASoulCharacter> CharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	TArray<ALobbyPlayerController*> PlayerControllers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	TArray<FPlayerProfile> PlayerProfiles;
	
	/** 플레이어가 리스폰될 지점 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	TArray<AActor*> SpawnPoints;
	
	////////////////////////////////////////////////////////////////////////////
	//// 서버 및 로비에서 사용됨
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	FText LobbyName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	int32 CurPlayer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	int32 MaxPlayer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
	UTexture2D* LobbyMapIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
	FText LobbyMapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
	FText LobbyMapTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
	int32 LobbyMapId;
	
	UFUNCTION(BlueprintCallable, Category = "Lobby")
    void UpdateLobbySettings(const FText& InName, const FText& InTime, int32 InId, UTexture2D* InIcon);

	void UpdateAll();
	void UpdateChatAll(const FText& Sender, const FText& Message);
	
	UFUNCTION(BlueprintCallable, Category = "Lobby")
    void KickPlayer(const int32 PlayerId);
	
public:
	////////////////////////////////////////////////////////////////////////////
	//// 오버라이딩
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	////////////////////////////////////////////////////////////////////////////
	//// 게터
	
	UFUNCTION(BlueprintPure, Category = "LobbyGameModeBase")
    bool IsStartableGame() const;

	////////////////////////////////////////////////////////////////////////////
	//// 기타 함수
	
	void RespawnPlayer(APlayerController* NewPlayer);
	
	UFUNCTION(BlueprintCallable, Category = "Lobby")
    void PlayGame();
};
