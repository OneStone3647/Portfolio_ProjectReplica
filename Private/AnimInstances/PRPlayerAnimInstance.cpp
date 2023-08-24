// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRPlayerAnimInstance.h"
#include "Characters/PRPlayerCharacter.h"

UPRPlayerAnimInstance::UPRPlayerAnimInstance()
{
	MoveForward = 0.0f;
	MoveRight = 0.0f;
}

void UPRPlayerAnimInstance::UpdateMovementInfo()
{
	Super::UpdateMovementInfo();

	APRPlayerCharacter* PRPlayerOwner = Cast<APRPlayerCharacter>(PROwner);
	if(IsValid(PRPlayerOwner) == true)
	{
		MoveForward = PRPlayerOwner->GetMoveForward();
		MoveRight = PRPlayerOwner->GetMoveRight();
	}
}
