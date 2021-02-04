// Fill out your copyright notice in the Description page of Project Settings.


#include "System/CharacterAnimInstance.h"
#include "System/SoulGameModeBase.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	OwnerPawn = TryGetPawnOwner();

	if(auto const GameMode = Cast<ASoulGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		FootStepSounds = GameMode->GetFootStepSounds();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(OwnerPawn)
	{
		const FVector Speed = OwnerPawn->GetVelocity();
		const FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		
		MoveSpeed = LateralSpeed.Size();
		Direction = CalculateDirection(Speed, OwnerPawn->GetActorRotation());
	}
}

void UCharacterAnimInstance::FootStep()
{
	FHitResult OutHit;
	FCollisionQueryParams TraceParms;
	TraceParms.AddIgnoredActor(OwnerPawn);
	
	const FVector StartLocation = OwnerPawn->GetActorLocation();

	GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation - FVector(0.f, 0.f, 200.f), StartLocation, ECollisionChannel::ECC_Visibility, TraceParms);
	
	if(OutHit.GetActor())
	{
		const int32 SurfaceType = static_cast<int32>(UGameplayStatics::GetSurfaceType(OutHit));
		
		if(SurfaceType < 6)
		{
			PlayerSound(SurfaceType);
		}
	}
}

void UCharacterAnimInstance::PlayerSound(const int32 SoundIndex)
{
	if(SoundIndex < FootStepSounds.Num())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FootStepSounds[SoundIndex], 0.75f, 1.f, 0.f);
	}
}
