// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SoulFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Engine/Engine.h"
#include "Containers/EnumAsByte.h"
#include "DrawDebugHelpers.h"

void USoulFunctionLibrary::CreateOverlapSphere(UWorld* World, FVector SphereLocation, float Radius, UClass* ClassFilter, AActor* IgnoreActor, TArray<AActor*>& OverlapActors)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*>IgnoreTypes;
	IgnoreTypes.Add(IgnoreActor);
	
	UKismetSystemLibrary::SphereOverlapActors(World, SphereLocation, Radius, ObjectTypes, ClassFilter, IgnoreTypes, OverlapActors);
	DrawDebugSphere(World, SphereLocation, Radius, 12, FColor::Red, false, 3.f, 2.f);
}

void USoulFunctionLibrary::DrawDLine(UWorld* World, FVector Start, FVector End)
{
	DrawDebugLine(World, Start, End, FColor::Orange, false, 1.f);
	DrawDebugSphere(World, End, 5.f, 8, FColor::Red, false, 1.f, 5.f);
	DrawDebugSphere(World, Start, 5.f, 8, FColor::Blue, false, 1.f, 5.f);
}
