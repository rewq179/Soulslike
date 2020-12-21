// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class SOULSLIKE_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxComponent;

public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Value")
	float BaseDamage;

	void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack();

public:	
	/** Returns CameraBoom subobject */
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }
};
