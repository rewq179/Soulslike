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
	// Sets default values for this component's properties
	UTargetComponent();

	/** 소유중인 Pawn */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	/** 소유중인 Pawn의 커스텀 Controller  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetComponent")
	float DetectRange;

	void Initialize();
	
protected:
	/** 바라볼 타겟 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	TArray<AActor*> TargetActors;

	/** 바라볼 타겟 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** True : 타게팅중, False : 일반  */
	//UPROPERTY(ReplicatedUsing = OnRep_Blocking, BlueprintReadOnly, Category = Movement)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Target)
	bool bTargeting;

public:
	void Targeting();
	void UnTargeting();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFindTarget();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLock(bool bLock);

	/** 네트워크 설정 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
