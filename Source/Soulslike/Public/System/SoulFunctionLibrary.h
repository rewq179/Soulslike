// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoulFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void CreateOverlapSphere(UWorld* World, FVector SphereLocation, float Radius, UClass* ClassFilter, AActor* IgnoreActor, TArray<AActor*>& OverlapActors);

	static void DrawDLine(UWorld* World, FVector Start, FVector End);

};
