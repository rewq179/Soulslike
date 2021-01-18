// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/InteractActor.h"
#include "InteractDoor.generated.h"

/**
 * 용도: 상호작용시 문이 열리거나 닫힌다.
 */

UCLASS()
class SOULSLIKE_API AInteractDoor : public AInteractActor
{
	GENERATED_BODY()
	
public:
	AInteractDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* DoorLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* DoorRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UBoxComponent* BoxComponent;

	virtual void Interact(ASoulCharacter* SoulCharacter) override;
	virtual void SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue) override;
	virtual FText GetInteractMessage() override;

protected:
	virtual void BeginPlay() override;

	/** 문이 완전히 열리는데 걸리는 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interact)
	float OpenTime;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Interact)
	bool bOpened;

public:
	void InteractDoor(bool bOpen);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDoorMoveTo(UStaticMeshComponent* Door, FVector Location, FRotator Rotator);
	
public:
	FORCEINLINE bool IsOpened() const { return bOpened; }

	UFUNCTION(BlueprintImplementableEvent)
	bool IsPlayerLocal(ASoulCharacter* Player);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
