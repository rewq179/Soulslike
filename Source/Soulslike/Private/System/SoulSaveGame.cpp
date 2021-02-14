// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SoulSaveGame.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

USoulSaveGame::USoulSaveGame()
{
	SaveSlotName = TEXT("SaveSlot");
	UserIndex = 0;
}

void USoulSaveGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USoulSaveGame, PlayerProfile);
}

void USoulSaveGame::SaveProfile(const FPlayerProfile& InProfile)
{
	PlayerProfile = InProfile;
	
	UGameplayStatics::SaveGameToSlot(this, SaveSlotName, UserIndex);
}

void USoulSaveGame::LoadProfile(FPlayerProfile& OutProfile) const
{
	OutProfile = PlayerProfile;
}
