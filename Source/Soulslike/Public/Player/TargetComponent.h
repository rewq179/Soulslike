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

	/** �������� Pawn */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulCharacter* OwnerCharacter;

	/** �������� Pawn�� Ŀ���� Controller  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	ASoulPlayerController* OwnerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetComponent")
	float DetectRange;

	void Initialize();
	
protected:
	/** �ٶ� Ÿ�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	TArray<AActor*> TargetActors;

	/** �ٶ� Ÿ�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target)
	AActor* Target;

	/** True : Ÿ������, False : �Ϲ�  */
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

	/** ��Ʈ��ũ ���� */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
