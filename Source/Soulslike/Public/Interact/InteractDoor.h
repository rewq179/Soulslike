// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/InteractActor.h"
#include "InteractDoor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AInteractDoor : public AInteractActor
{
	GENERATED_BODY()
	
	FTimerHandle DoorTimer;

public:
	AInteractDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* DoorLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* DoorRight;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Opening, VisibleAnywhere, BlueprintReadOnly, Category = Door)
	bool bOpened;

	/** 문이 완전히 열리는데 걸리는 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Door)
	float OpenTime;

	UFUNCTION()
	void OnRep_Opening();

public:
	void StartTimer(bool bOpen);

	void InteractDoor();
	void CloseDoor();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastDoorMoveTo(UStaticMeshComponent* Door, FVector Location, FRotator Rotator);

public:
	FORCEINLINE bool IsOpening() { return bOpened; }

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
