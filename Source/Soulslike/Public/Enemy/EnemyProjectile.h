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

UCLASS()
class SOULSLIKE_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	USceneComponent* DefaultSceneRoot;

	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovement;

public:	
	// Sets default values for this actor's properties
	AEnemyProjectile();

	/** �߻�ü�� ũ�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* SphereComponent;

	/** �߻�ü�� ���󰡰� �ִ� ���� ����� ��ƼŬ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle)
	UParticleSystemComponent* Particle;

	/** �߻�ü�� ����� ����� �� ������ ��ƼŬ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particle)
	UParticleSystem* HitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<UDamageType> DamageType;

	/** �߻�ü�� ��󿡰� ���� ������ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float Damage;

	/** True : ����� ����, False : ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	bool bHit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnActorDestroyed(AActor* DestroyedActor);

	/** �߻�ü�� ����� ���߸� �����Ѵ�. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnFX();

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
