// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRBaseAnimInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

UPRBaseAnimInstance::UPRBaseAnimInstance()
{
	// CharacterReference
	PROwner = nullptr;

	// MovementInfo
	MovementState = EPRMovementState::MovementState_None;
	PreviousMovementState = EPRMovementState::MovementState_None;
	Gait = EPRGait::Gait_Walking;
	Speed = 0.0f;
	WalkSpeed = 0.0f;
	AnimatedWalkSpeed = 220.0f;
	AnimatedRunSpeed = 600.0f;
	AnimatedSprintSpeed = 800.0f;
	bIsMoving = false;
	bIsAccelerating = false;
	SpeedWhenStopping = 0.0f;
	MovePlayRate = 0.0f;
	Direction = 0.0f;

	// CharacterState
	bIsDead = false;
}

void UPRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(TryGetPawnOwner());
	if(IsValid(NewPROwner) == true)
	{
		PROwner = NewPROwner;
		
		UpdateMovementInfo();
		UpdateCharacterState();
	}
}

#pragma region MovementInfo
void UPRBaseAnimInstance::UpdateMovementInfo()
{
	MovementState = PROwner->GetMovementSystem()->GetMovementState();
	PreviousMovementState = PROwner->GetMovementSystem()->GetPreviousMovementState();
	Gait = PROwner->GetMovementSystem()->GetGait();
	Speed = PROwner->GetMovementSystem()->GetSpeed();
	bIsMoving = PROwner->GetMovementSystem()->IsMoving();
	bIsAccelerating = PROwner->GetMovementSystem()->IsAccelerating();
	SpeedWhenStopping = PROwner->GetMovementSystem()->GetSpeedWhenStopping();
	WalkSpeed = PROwner->GetMovementSystem()->GetWalkSpeed();
	MovePlayRate = CalculateMovePlayRate();
	Direction = CalculateDirection(PROwner->GetVelocity(), PROwner->GetActorRotation());
}

float UPRBaseAnimInstance::CalculateMovePlayRate() const
{
	return UKismetMathLibrary::FClamp(((Speed / AnimatedWalkSpeed) / GetOwningComponent()->GetComponentScale().Z), 0.0f, 1.0f);
}
#pragma endregion 

#pragma region CharacterState
void UPRBaseAnimInstance::UpdateCharacterState()
{
	bIsDead = PROwner->GetStateSystem()->IsDead();
}
#pragma endregion 
