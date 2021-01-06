// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataType.h"
#include "InteractActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UBoxComponent;
class ASoulCharacter;

UCLASS()
class SOULSLIKE_API AInteractActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interact)
	FString Name;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Interact)
	EInteractType InteractType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interact)
	bool bInteracted;

public:	
	AInteractActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact();
	virtual void SetRenderCustomDepth(ASoulCharacter* InPlayer, bool bTrue);
	virtual FText GetInteractMessage();

public:
	FORCEINLINE bool IsInteracted() const { return bInteracted; }
	FORCEINLINE EInteractType GetInteractType() const { return InteractType; }

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
