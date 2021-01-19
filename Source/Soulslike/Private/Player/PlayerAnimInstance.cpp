
#include "Player/PlayerAnimInstance.h"

#include "Player/SoulCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Pawn = TryGetPawnOwner();

	if(auto const SoulCharacter = Cast<ASoulCharacter>(Pawn))
	{
		SoulCharacter->SetPlayerAnimInstance(this);
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(Pawn)
	{
		const FVector Speed = Pawn->GetVelocity();
		const FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		
		MoveSpeed = LateralSpeed.Size();
	}
}
