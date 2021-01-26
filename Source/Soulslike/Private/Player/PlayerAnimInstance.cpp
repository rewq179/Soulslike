
#include "Player/PlayerAnimInstance.h"

#include "Player/SoulCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if(auto const SoulCharacter = Cast<ASoulCharacter>(OwnerPawn))
	{
		SoulCharacter->SetPlayerAnimInstance(this);
	}
}