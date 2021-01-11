// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;
class AInteractActor;
class APickUpActor;
class AInteractDoor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	void Initialize();


protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PickUpActor)
	APickUpActor* CurrentPickUpActor;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PickUpActor)
	AInteractDoor* CurrentDoor;
		
public:
	void InteractActor();
	void SetPickUpActor(APickUpActor* PickUpActor);
	void SetInteractDoor(AInteractDoor* DoorActor);

	void HandlePickUp();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	AInteractActor* CurActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	AInteractActor* LastActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	bool bActorChanged;

public:
	void CheckInteractActor();
	void SetInteractActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
