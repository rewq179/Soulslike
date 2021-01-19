// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

/**
* 용도: 플레이어의 상호작용(물체, 루팅 아이템, 문)
*
* 플레이어의 시야에서 200 이내 물체가 상호작용이 가능하다면 외각선이 강조된다
* 플레이어가 상호 작용(F 키)를 입력하면 상호작용이 된다.
*/

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
	FORCEINLINE void SetInteractDoor(AInteractDoor* DoorActor) {CurrentDoor = DoorActor;}
	FORCEINLINE void SetPickUpActor(APickUpActor* PickUpActor) {CurrentPickUpActor = PickUpActor;}

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
