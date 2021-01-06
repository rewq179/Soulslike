// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetComponent();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetComponent")
	float DetectRange;

	void Initialize();
	
protected:
	/** 찾아낸 Enemy 들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	TArray<AActor*> TargetActors;

	/** 바라볼 타겟 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** True : 타게팅중, False : 일반  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Target)
	bool bTargeting;

public:
	void Targeting();
	void UnTargeting();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFindTarget();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLock(bool bLock);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
