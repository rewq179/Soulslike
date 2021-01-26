// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"

#include "Enemy/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if(auto const Enemy = Cast<AEnemy>(OwnerPawn))
	{
		Enemy->SetEnemyAnimInstance(this);
	}
}
