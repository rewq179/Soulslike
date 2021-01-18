// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class USceneComponent;
class USphereComponent;
class UParticleSystemComponent;
class UParticleSystem;
class UProjectileMovementComponent;
class UDamageType;

/**
* 용도: Enemy가 보유한 원거리 공격의 투사체
*/

UCLASS()
class SOULSLIKE_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	USceneComponent* DefaultSceneRoot;

	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovement;

protected :	
	AEnemyProjectile();

	/** 발사체의 반응되는 영역 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComponent;

	/** 발사체가 날라가고 있는 동안 재생할 파티클 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle)
	UParticleSystemComponent* Particle;

	/** 발사체가 대상을 맞췄을 때 보여줄 파티클 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle)
	UParticleSystem* HitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float Damage;

	/** True : 대상을 맞춤, False : 못맞춤 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	bool bHit;

	virtual void BeginPlay() override;

public:	
	FORCEINLINE void SetDamage(const float InDamage) {Damage = InDamage;}

	/** 발사체가 대상을 맞추면 실행한다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayParticle();

	UFUNCTION()
    void OnActorDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
