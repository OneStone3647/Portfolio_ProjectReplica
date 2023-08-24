// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/PRBaseAnimInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UPRBaseAnimInstance::UPRBaseAnimInstance()
{
	// CharacterReference
	PROwner = nullptr;

	// CharacterState
	bIsDead = false;

	// MovementInfo
	MovementState = EPRMovementState::MovementState_None;
	PreviousMovementState = EPRMovementState::MovementState_None;
	bJumped = false;
	FootDown = EPRFoot::Foot_Left;
	Gait = EPRGait::Gait_Running;
	PreviousGait = EPRGait::Gait_None;
	MaxWalkSpeed = 0.0f;
	Velocity = FVector::ZeroVector;
	SpeedWhenStop = 0.0f;
	SpeedWhenJump = 0.0f;
	bIsMoving = false;
	CurrentAcceleration = FVector::ZeroVector;

	// FootIK
	bActiveFootIK = false;
	bActiveFootIKDebug = false;
	FootIKTraceDistance = 55.0f;
	FootIKTraceRadius = 10.0f;
	LeftFootBoneName = FName("foot_l");
	RightFootBoneName = FName("foot_r");
	AdjustFootOffset = 2.5f;
	FootInterpSpeed = 22.0f;
	HipsOffsetInterpSpeed = 17.0f;
	LeftFootEffectorLocationOffset = 0.0f;
	RightFootEffectorLocationOffset = 0.0f;
	LeftFootEffectorLocation = FVector::ZeroVector;
	RightFootEffectorLocation = FVector::ZeroVector;
	HipsOffset = 0.0f;
	OwnerInitializeCapsuleHalfHeight = 0.0f;
	LeftFootRotation = FRotator::ZeroRotator;
	RightFootRotation = FRotator::ZeroRotator;
	JointTargetLeft = FVector(50.0f, 217.0f, -38.0f);
	JointTargetRight = FVector(50.f, 217.0f, 38.0f);
}

void UPRBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// CharacterReference
	InitializePROwner();

	// FootIK
	OwnerInitializeCapsuleHalfHeight = PROwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void UPRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(PROwner != nullptr)
	{
		// CharacterState
		UpdateCharacterState();

		// MovementInfo
		UpdateMovementInfo();
	
		// FootIK
		UpdateFootIK(DeltaSeconds);
		UpdateFootIKDebug();
	}
}

#pragma region CharacterReference
void UPRBaseAnimInstance::InitializePROwner()
{
	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(TryGetPawnOwner());
	if(IsValid(NewPROwner) == true)
	{
		PROwner = NewPROwner;
	}
}
#pragma endregion

#pragma region CharacterState
void UPRBaseAnimInstance::UpdateCharacterState()
{
	bIsDead = PROwner->GetStateSystem()->IsDead();
}
#pragma endregion 

#pragma region MovementInfo
void UPRBaseAnimInstance::UpdateMovementInfo()
{
	if(IsValid(PROwner) == true)
	{
		MovementState = PROwner->GetMovementSystem()->GetMovementState();
		PreviousMovementState = PROwner->GetMovementSystem()->GetPreviousMovementState();
		bJumped = PROwner->IsJumped();
		SetAllowedGait(PROwner->GetMovementSystem()->GetAllowedGait());
		SetGait(PROwner->GetMovementSystem()->GetGait());
		MaxWalkSpeed = PROwner->GetMovementSystem()->GetMaxWalkSpeed(PROwner->GetMovementSystem()->GetGait());
		bIsMoving = PROwner->GetMovementSystem()->IsMoving();
		Velocity = PROwner->GetVelocity();
		SpeedWhenStop = PROwner->GetMovementSystem()->GetSpeedWhenStop();
		SpeedWhenJump = PROwner->GetMovementSystem()->GetSpeedWhenJump();
		CurrentAcceleration = PROwner->GetCharacterMovement()->GetCurrentAcceleration();
	}
}

bool UPRBaseAnimInstance::IsEqualMovementState(EPRMovementState NewPRMovementState) const
{
	return MovementState == NewPRMovementState;
}

void UPRBaseAnimInstance::SetFootDown(EPRFoot NewFoot)
{
	FootDown = NewFoot;
}

void UPRBaseAnimInstance::SetAllowedGait(EPRGait NewAllowedGait)
{
	if(AllowedGait == NewAllowedGait)
	{
		return;
	}
	
	PreviousAllowedGait = AllowedGait;
	AllowedGait = NewAllowedGait;
}

void UPRBaseAnimInstance::SetGait(EPRGait NewGait)
{
	if(Gait == NewGait)
	{
		return;
	}
	
	PreviousGait = Gait;
	Gait = NewGait;
}
#pragma endregion 

#pragma region FootIK
void UPRBaseAnimInstance::InitializeFootIK()
{
	if(IsValid(PROwner) == true && PROwner->GetMesh()->SkeletalMesh != nullptr)
	{		
		// FootIK의 값을 초기화합니다.
		UpdateFootIKOffset(0.0f, LeftFootEffectorLocationOffset, GetWorld()->GetDeltaSeconds(), FootInterpSpeed);
		UpdateFootIKOffset(0.0f, RightFootEffectorLocationOffset, GetWorld()->GetDeltaSeconds(), FootInterpSpeed);

		UpdateFootIKOffset(0.0f, HipsOffset, GetWorld()->GetDeltaSeconds(), HipsOffsetInterpSpeed);

		UpdateFootIKRotation(FRotator::ZeroRotator, LeftFootRotation, GetWorld()->GetDeltaSeconds(), FootInterpSpeed);
		UpdateFootIKRotation(FRotator::ZeroRotator, RightFootRotation, GetWorld()->GetDeltaSeconds(), FootInterpSpeed);

		UpdateCapsuleHalfHeight(0.0f, GetWorld()->GetDeltaSeconds(), true);

		LeftFootEffectorLocation = FVector::ZeroVector;
		RightFootEffectorLocation = FVector::ZeroVector;
	}
}

void UPRBaseAnimInstance::UpdateFootIKDebug()
{
	// FootIK의 값의 디버그를 출력합니다.
	if(bActiveFootIKDebug == true)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "LeftFoot Effector Location(Z): " + FString::SanitizeFloat(LeftFootEffectorLocationOffset), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "RightFoot Effector Location(Z): " + FString::SanitizeFloat(RightFootEffectorLocationOffset), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "HipsOffset(Z): " + FString::SanitizeFloat(HipsOffset), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "LeftFoot Rotation: " + LeftFootRotation.ToString(), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "RightFoot Rotation: " + RightFootRotation.ToString(), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "==FootIK Debug==", true, false, FLinearColor::Green, 0.0f);
	}

	// 캐릭터의 CapsuleCollision Debug
	if(PROwner != nullptr)
	{
		PROwner->GetCapsuleComponent()->SetHiddenInGame(!bActiveFootIKDebug);
	}
}

void UPRBaseAnimInstance::UpdateFootIK(float DeltaTime)
{
	if(PROwner == nullptr)
	{
		PR_LOG_WARNING("PROwner is NULL");
		return;
	}

	// 공중에 존재하지 않고 전력질주 상태가 아닐 때 FootIk를 실행합니다.
	if(bActiveFootIK == true && IsEqualMovementState(EPRMovementState::MovementState_InAir) == false && Gait != EPRGait::Gait_Sprinting)
	{
		FHitResult LeftFootTraceHitResult;
		FHitResult RightFootTraceHitResult;

		// Update FootIKTrace
		UpdateFootIKTrace(LeftFootTraceHitResult, LeftFootEffectorLocationOffset, LeftFootBoneName, bActiveFootIKDebug, FootIKTraceDistance, FootIKTraceRadius);
		UpdateFootIKTrace(RightFootTraceHitResult, RightFootEffectorLocationOffset, RightFootBoneName, bActiveFootIKDebug, FootIKTraceDistance, FootIKTraceRadius);

		// Update FootRotation
		if(bIsMoving)
		{
			UpdateFootIKRotation(FRotator::ZeroRotator, LeftFootRotation, DeltaTime, FootInterpSpeed);
			UpdateFootIKRotation(FRotator::ZeroRotator, RightFootRotation, DeltaTime, FootInterpSpeed);
		}
		else
		{
			UpdateFootIKRotation(CalculateNormalToRotator(LeftFootTraceHitResult.ImpactNormal), LeftFootRotation, DeltaTime, FootInterpSpeed);
			UpdateFootIKRotation(CalculateNormalToRotator(RightFootTraceHitResult.ImpactNormal), RightFootRotation, DeltaTime, FootInterpSpeed);
		}
		
		// Update HipsOffset(Z)
		float NewHipsOffset = UKismetMathLibrary::Min(LeftFootEffectorLocationOffset, RightFootEffectorLocationOffset);
		if(NewHipsOffset > 0)
		{
			NewHipsOffset = 0.0f;
		}
		UpdateFootIKOffset(NewHipsOffset, HipsOffset, DeltaTime, HipsOffsetInterpSpeed);

		// Update CapsuleHalfHeight;
		UpdateCapsuleHalfHeight(HipsOffset, DeltaTime, false);

		// Update FootOffset
		UpdateFootIKOffset(LeftFootEffectorLocationOffset - HipsOffset, LeftFootEffectorLocation.X, DeltaTime, FootInterpSpeed);
		UpdateFootIKOffset(-1 * (RightFootEffectorLocationOffset - HipsOffset), RightFootEffectorLocation.X, DeltaTime, FootInterpSpeed);
	}
	else
	{
		InitializeFootIK();
	}
}

bool UPRBaseAnimInstance::UpdateFootIKTrace(FHitResult& HitResult, float& FootIKOffset, FName SocketName, bool bDebugFlag, float TraceDistance, float TraceRadius)
{
	if(PROwner == nullptr)
	{
		PR_LOG_WARNING("PROwner is NULL");
		return false;
	}

	const FVector FootLocation = PROwner->GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart = FVector(FootLocation.X, FootLocation.Y, PROwner->GetActorLocation().Z);
	const FVector TraceEnd = FVector(FootLocation.X, FootLocation.Y, (PROwner->GetActorLocation().Z - OwnerInitializeCapsuleHalfHeight) - TraceDistance);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(PROwner);

	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if(bDebugFlag)
	{
		DebugType = EDrawDebugTrace::ForOneFrame;
	}

	bool bFootIKTraceHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceRadius,
																	UEngineTypes::ConvertToTraceType(ECC_Visibility),
																	false, ActorsToIgnore, DebugType, HitResult, true);

	if(HitResult.IsValidBlockingHit() == true)
	{
		float ImpactLenght = (HitResult.ImpactPoint - HitResult.TraceEnd).Size();
		FootIKOffset = AdjustFootOffset + (ImpactLenght - TraceDistance);
	}
	else
	{
		FootIKOffset = 0.0f;
	}

	return bFootIKTraceHit;
}

void UPRBaseAnimInstance::UpdateFootIKOffset(float TargetOffset, float& CurrentOffset, float DeltaTime, float InterpSpeed)
{
	float InterpOffset = UKismetMathLibrary::FInterpTo(CurrentOffset, TargetOffset, DeltaTime, InterpSpeed);
	CurrentOffset = InterpOffset;
}

void UPRBaseAnimInstance::UpdateFootIKRotation(FRotator TargetRotation, FRotator& CurrentRotation, float DeltaTime, float InterpSpeed)
{
	FRotator InterpRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
	CurrentRotation = InterpRotation;
}

void UPRBaseAnimInstance::UpdateCapsuleHalfHeight(float CurrentHipsOffset, float DeltaTime, bool bInitializeFlag)
{
	if(PROwner == nullptr)
	{
		PR_LOG_WARNING("PROwner is NULL");
		return;
	}

	UCapsuleComponent* CapsuleComponent = PROwner->GetCapsuleComponent();
	if(CapsuleComponent == nullptr)
	{
		PR_LOG_WARNING("CapsuleComponent is NULL");
		return;
	}

	float NewCapsuleHalfHeight = 0.0f;
	if(bInitializeFlag)
	{
		NewCapsuleHalfHeight = OwnerInitializeCapsuleHalfHeight;
	}
	else
	{
		float HalfAbsSize = UKismetMathLibrary::Abs(CurrentHipsOffset) * 0.5f;
		NewCapsuleHalfHeight = OwnerInitializeCapsuleHalfHeight - HalfAbsSize;
	}

	float ScaledCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float InterpCapsuleHalfHeight = UKismetMathLibrary::FInterpTo(ScaledCapsuleHalfHeight, NewCapsuleHalfHeight, DeltaTime, HipsOffsetInterpSpeed);

	CapsuleComponent->SetCapsuleHalfHeight(InterpCapsuleHalfHeight);
}

FRotator UPRBaseAnimInstance::CalculateNormalToRotator(FVector NormalVector)
{
	float FirstAtan2 = UKismetMathLibrary::DegAtan2(NormalVector.Y, NormalVector.Z);
	float SecondAtan2 = UKismetMathLibrary::DegAtan2(NormalVector.X, NormalVector.Z);
	SecondAtan2 *= -1.0f;
	
	FRotator ResultRotation = FRotator(SecondAtan2, 0.0f, FirstAtan2);

	return ResultRotation;
}

void UPRBaseAnimInstance::SetActiveFootIK(bool bFlag)
{
	bActiveFootIK = bFlag;
}
#pragma endregion 
