// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "Enemy/Enemy.h"
#include "Perception/PawnSensingComponent.h"

AEnemyAIController::AEnemyAIController()
{
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	PawnSensing->SightRadius = 1500.f;
	PawnSensing->SetPeripheralVisionAngle(180.f);
}

// Called when the game starts or when spawned
void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyAIController::OnSeePawn);
}

void AEnemyAIController::OnSeePawn(APawn* InPawn)
{
	if (Target == nullptr)
	{
		Target = InPawn;

		auto Enemy = Cast<AEnemy>(GetCharacter());
		if (Enemy)
		{
			Enemy->SetTarget(Target);
		}
	}
}
