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
	// Sets default values for this component's properties
	UInteractComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** �������� Pawn */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	/** �������� Pawn�� Ŀ���� Controller  */
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
	UPROPERTY(ReplicatedUsing = OnRep_Weapon, BlueprintReadOnly, Category = Equip)
	AActor* CurrentWeapon;

	/** RefNotify */
	UFUNCTION()
	void OnRep_Weapon();

	/** BP���� ABP_SoulCharacter�� bool�� ���� */
	UFUNCTION(BlueprintImplementableEvent, Category = Equip)
	void OnUpdateWeapon();

public:
	void EquipWeapon(AActor* Item);

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

public:
	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
