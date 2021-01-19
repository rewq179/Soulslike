// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetComponent.generated.h"

class ASoulCharacter;
class ASoulPlayerController;

/**
* 용도: 플레이어가 Enemy를 타겟팅해 시야를 고정시키기 위함
*
* 플레이어가 마우스 휠을 아래로 내리면 근처 1200 이내 몬스터를 타겟팅한다.
* 타겟팅된 몬스터의 내부에 ㅇ 위젯이 표시된다
* 플레이어의 카메라는 Enemy를 향해 회전한다.
*/

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

	/** 탐지 가능한 거리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	float DetectRange;

	void Initialize();
	
protected:
	/** 찾아낸 Enemy 들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	TArray<AActor*> TargetActors;

	/** 바라볼 타겟 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	AActor* Target;

	/** True : 타게팅중, False : 일반  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	bool bTargeting;

	UFUNCTION(Server, Reliable)
    void ServerFindTarget();

	UFUNCTION(Server, Reliable)
    void ServerSetLock(const bool bLock);
	
public:
	void Targeting();
	void UnTargeting();
	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
