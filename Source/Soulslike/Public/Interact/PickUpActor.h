// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/InteractActor.h"
#include "DataType.h"
#include "PickUpActor.generated.h"

/**
 * 용도: 아이템 근처에서 상호작용[F]을 하면 아이템을 획득할 수 있다.
 */

UCLASS()
class SOULSLIKE_API APickUpActor : public AInteractActor
{
	GENERATED_BODY()
	
public:	
	APickUpActor();
	
	virtual void Interact(ASoulCharacter* SoulCharacter) override;
	virtual void SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue) override;
	virtual FText GetInteractMessage() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interact)
	FPickUpInfo PickUpInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interact)
	FItemTable ItemTable;

	class UDataTable* ItemDataTable;
	
public:
	FORCEINLINE FPickUpInfo GetPickUpInfo() const {return PickUpInfo;}
	FORCEINLINE FItemTable GetItemTable() const {return ItemTable;}
	FORCEINLINE EPickUpType GetPickUpType() const {return PickUpInfo.PickUpType;}

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};
