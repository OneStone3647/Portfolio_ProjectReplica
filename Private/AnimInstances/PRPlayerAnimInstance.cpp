// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRPlayerAnimInstance.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"

UPRPlayerAnimInstance::UPRPlayerAnimInstance()
{
	// CharacterReference
	PRPlayerOwner = nullptr;
	
	// CharacterInfo
	bIsDoubleJump = false;
	bIsLockOnTarget = false;
	bWasAerial = false;
}

void UPRPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	APRPlayerCharacter* NewPRPlayerOwner = Cast<APRPlayerCharacter>(TryGetPawnOwner());
	if(IsValid(NewPRPlayerOwner) == true)
	{
		PRPlayerOwner = NewPRPlayerOwner;

		UpdatePlayerMovementInfo();
	}
}

void UPRPlayerAnimInstance::UpdatePlayerMovementInfo()
{
	bIsDoubleJump = PRPlayerOwner->IsDoubleJump();
	bIsLockOnTarget = PRPlayerOwner->GetTargetingSystem()->IsLockOnTarget();
	bWasAerial = PRPlayerOwner->GetMovementSystem()->WasAerial();
}
