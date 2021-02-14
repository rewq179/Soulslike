// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataType.h"
#include "Player/ActorComponent/StatComponent.h"

#include "SoulFunctionLibrary.generated.h"

class UDamageType;
class AController;

/**
 * 용도 : 클래스에서 공통으로 사용되는 함수들을 모아놓음
 */

UCLASS()
class SOULSLIKE_API USoulFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	////////////////////////////////////////////////////////////////////////////
	//// 플레이어, 에너미
	
	// OverlapSphere를 생성한다. Enemy와 SoulCharacter에서 사용된다.
	static void CreateOverlapSphere(UWorld* World, const FVector& SphereLocation, float Radius, UClass* ClassFilter, AActor* IgnoreActor, TArray<AActor*>& OverlapActors);

	// OverlapSphere를 생성시 형태를 그려준다.
	static void DrawDLine(UWorld* World, FVector Start, FVector End);

	// 몬스터가 플레이어에게 데미지를 준다. Enemy와 EnemyProjectile 클래스에서 사용된다.
	static void ApplyDamageToPlayer(AActor* InPlayer, float Damage, AController* Controller, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType, float Height, bool bKnockDown);

	static void ApplyPotion(UStatComponent* StatComponent, FItemTable& Item);
	static void ApplyEquipmentStat(UStatComponent* StatComponent, FItemTable& Item, const bool bEquip);

    ////////////////////////////////////////////////////////////////////////////
	//// 게터

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject"))
	static class USoulGameInstance* GetSoulGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject"))
	static class ALobbyGameModeBase* GetLobbyGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject"))
	static class ASoulGameModeBase* GetSoulGameMode(const UObject* WorldContextObject);
};
