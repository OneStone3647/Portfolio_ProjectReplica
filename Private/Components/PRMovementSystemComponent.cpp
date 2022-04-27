// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UPRMovementSystemComponent::UPRMovementSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// MovementInfo
	MovementState = EPRMovementState::MovementState_None;
	PreviousMovementState = EPRMovementState::MovementState_None;
	RotationMode = EPRRotationMode::RotationMode_VelocityDirection;
	Gait = EPRGait::Gait_Walking;
	AllowedGait = EPRGait::Gait_Running;
	PreviousAllowedGait = EPRGait::Gait_Walking;
	bCanMove = true;
	bIsMoving = false;
	bIsAccelerating = false;
	bIsSetSpeedWhenStopping = false;
	SpeedWhenStopping = 0.0f;
	MinAnalogWalkSpeed = 180.0f;
	Speed = 0.0f;
	WalkSpeed = 220.0f;
	RunSpeed = 600.0f;
	SprintSpeed = 1000.0f;
	DecreaseRunToWalkSpeedCurveFloat = nullptr;
	DecreaseSprintToRunSpeedCurveFloat = nullptr;
	DecreaseSprintToWalkSpeedCurveFloat = nullptr;

	// GravityControl
	GravityControlDelayTime = 0.5f;
	IncreaseGravityScale = 3.0f;
	bIsAerial = false;
	bWasAerial = false;
}

void UPRMovementSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaxWalkSpeedTimeline();
}

void UPRMovementSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DecreaseMaxWalkSpeedTimeline.TickTimeline(DeltaTime);
	
	UpdateMovementInfo();
	UpdateGravityControlInTheAir();
}

void UPRMovementSystemComponent::OnCharacterMovementModeChanged(EMovementMode NewMovementMode)
{
	switch(NewMovementMode)
	{
	case EMovementMode::MOVE_Walking:
	case EMovementMode::MOVE_NavWalking:
		SetMovementState(EPRMovementState::MovementState_Ground);
		break;
	case EMovementMode::MOVE_Falling:
		SetMovementState(EPRMovementState::MovementState_InAir);
		break;
	default:
		break;
	}
}

bool UPRMovementSystemComponent::IsEqualMovementState(EPRMovementState NewPRMovementState) const
{
	return MovementState == NewPRMovementState;
}

bool UPRMovementSystemComponent::IsEqaulGait(EPRGait NewPRGait) const
{
	return Gait == NewPRGait;
}

bool UPRMovementSystemComponent::IsCanMove() const
{
	return bCanMove;
}

bool UPRMovementSystemComponent::IsMoving() const
{
	return bIsMoving;
}

bool UPRMovementSystemComponent::IsAccelerating() const
{
	return bIsAccelerating;
}

void UPRMovementSystemComponent::UpdateMovementInfo()
{
	if(PROwner->GetVelocity() != FVector::ZeroVector)
	{
		bIsMoving = true;
	}
	else
	{
		bIsMoving = false;		
	}
	
	// UKismetMathLibrary::VSizeXY: 벡터의 XY의 길이를 반환합니다.
	Speed = UKismetMathLibrary::VSizeXY(PROwner->GetVelocity());
	SetCharacterMaxWalkSpeed(AllowedGait);
	SetGait(Speed);
	
	// float의 값을 0과 비교할 때 미세한 오차 범위 내에 있는지를 판단하는 것이 좋습니다.
	// 무시 가능한 오차를 측정할 때 KINDA_SMALL_NUMBER 매크로를 사용합니다.
	bIsAccelerating = PROwner->GetCharacterMovement()->GetCurrentAcceleration().Size() > KINDA_SMALL_NUMBER;

	SetSpeedWhenStopping();
}

void UPRMovementSystemComponent::SetCharacterMaxWalkSpeed(EPRGait NewPRGait)
{
	SetDecreaseMaxWalkSpeedCurve();

	if(DecreaseMaxWalkSpeedTimeline.IsPlaying() == false)
	{
		switch(NewPRGait)
		{
		case EPRGait::Gait_Walking:
			if(PROwner->GetCharacterMovement()->MaxWalkSpeed != WalkSpeed)
			{
				DecreaseMaxWalkSpeedTimeline.PlayFromStart();
			}
			break;
		case EPRGait::Gait_Running:
			if(PROwner->GetCharacterMovement()->MaxWalkSpeed != RunSpeed)
			{
				if(PreviousAllowedGait == EPRGait::Gait_Sprinting)
				{
					DecreaseMaxWalkSpeedTimeline.PlayFromStart();
				}
				else if(PreviousAllowedGait == EPRGait::Gait_Walking)
				{
					DecreaseMaxWalkSpeedTimeline.ReverseFromEnd();
				}
			}
			break;
		case EPRGait::Gait_Sprinting:
			if(PROwner->GetCharacterMovement()->MaxWalkSpeed != SprintSpeed)
			{
				DecreaseMaxWalkSpeedTimeline.ReverseFromEnd();
			}
			break;
		}
	}
}

void UPRMovementSystemComponent::InitializeMaxWalkSpeedTimeline()
{
	if(DecreaseRunToWalkSpeedCurveFloat && DecreaseSprintToRunSpeedCurveFloat && DecreaseSprintToWalkSpeedCurveFloat)
	{
		// Curve가 사용할 Callback 함수입니다.
		FOnTimelineFloat TimelineProgress;

		// Callback 함수에서 사용할 함수를 바인딩합니다.
		TimelineProgress.BindUFunction(this, FName("DecreaseCharacterMaxWalkSpeed"));

		// Timeline에 Curve를 추가합니다.
		DecreaseMaxWalkSpeedTimeline.AddInterpFloat(DecreaseSprintToRunSpeedCurveFloat, TimelineProgress, NAME_None, TEXT("DecreaseMaxWalkSpeed"));
	}
}

void UPRMovementSystemComponent::DecreaseCharacterMaxWalkSpeed(float Value)
{
	PROwner->GetCharacterMovement()->MaxWalkSpeed = Value;

	float AllowedMaxWalkSpeed = 0.0f;
	switch(AllowedGait)
	{
	case EPRGait::Gait_Walking:
		AllowedMaxWalkSpeed = WalkSpeed;
		break;
	case EPRGait::Gait_Running:
		AllowedMaxWalkSpeed = RunSpeed;
		break;
	case EPRGait::Gait_Sprinting:
		AllowedMaxWalkSpeed = SprintSpeed;
		break;
	}
	
	if(PROwner->GetCharacterMovement()->MaxWalkSpeed == AllowedMaxWalkSpeed)
	{
		DecreaseMaxWalkSpeedTimeline.Stop();
	}
}

void UPRMovementSystemComponent::SetDecreaseMaxWalkSpeedCurve()
{
	switch(AllowedGait)
	{
	case EPRGait::Gait_Walking:
		if(PreviousAllowedGait == EPRGait::Gait_Running && DecreaseRunToWalkSpeedCurveFloat)
		{
			DecreaseMaxWalkSpeedTimeline.SetFloatCurve(DecreaseRunToWalkSpeedCurveFloat, TEXT("DecreaseMaxWalkSpeed"));
		}
		break;
	case EPRGait::Gait_Running:
		if(PreviousAllowedGait == EPRGait::Gait_Sprinting && DecreaseSprintToRunSpeedCurveFloat)
		{
			DecreaseMaxWalkSpeedTimeline.SetFloatCurve(DecreaseSprintToRunSpeedCurveFloat, TEXT("DecreaseMaxWalkSpeed"));
		}
		else if(PreviousAllowedGait == EPRGait::Gait_Walking && DecreaseRunToWalkSpeedCurveFloat)
		{
			DecreaseMaxWalkSpeedTimeline.SetFloatCurve(DecreaseRunToWalkSpeedCurveFloat, TEXT("DecreaseMaxWalkSpeed"));
		}
		break;
	case EPRGait::Gait_Sprinting:
		if(PreviousAllowedGait == EPRGait::Gait_Running && DecreaseSprintToRunSpeedCurveFloat)
		{
			DecreaseMaxWalkSpeedTimeline.SetFloatCurve(DecreaseSprintToRunSpeedCurveFloat, TEXT("DecreaseMaxWalkSpeed"));
		}
		else if(PreviousAllowedGait == EPRGait::Gait_Walking && DecreaseSprintToWalkSpeedCurveFloat)
		{
			DecreaseMaxWalkSpeedTimeline.SetFloatCurve(DecreaseSprintToWalkSpeedCurveFloat, TEXT("DecreaseMaxWalkSpeed"));
		}
	default:
		break;
	}
}

EPRMovementState UPRMovementSystemComponent::GetMovementState() const
{
	return MovementState;
}

void UPRMovementSystemComponent::SetMovementState(EPRMovementState NewPRMovementState)
{
	PreviousMovementState = MovementState;
	MovementState = NewPRMovementState;
}

EPRMovementState UPRMovementSystemComponent::GetPreviousMovementState() const
{
	return PreviousMovementState;
}

void UPRMovementSystemComponent::SetRotationMode(EPRRotationMode NewRotationMode)
{
	RotationMode = NewRotationMode;
	switch(RotationMode)
	{
	case EPRRotationMode::RotationMode_VelocityDirection:
		PROwner->GetCharacterMovement()->bOrientRotationToMovement = true;
		PROwner->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EPRRotationMode::RotationMode_LookingDirection:
		PROwner->GetCharacterMovement()->bOrientRotationToMovement = false;
		PROwner->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		break;
	default:
		break;
	}
}

EPRGait UPRMovementSystemComponent::GetGait() const
{
	return Gait;
}

void UPRMovementSystemComponent::SetGait(float NewSpeed)
{
	if(NewSpeed > RunSpeed + 5.0f)
	{
		Gait = EPRGait::Gait_Sprinting;
	}
	else if(NewSpeed > WalkSpeed + 5.0f)
	{
		Gait = EPRGait::Gait_Running;
	}
	else
	{
		Gait = EPRGait::Gait_Walking;
	}
}

EPRGait UPRMovementSystemComponent::GetAllowedGait() const
{
	return AllowedGait;
}

void UPRMovementSystemComponent::SetAllowedGait(EPRGait NewPRGait)
{
	PreviousAllowedGait = AllowedGait;
	AllowedGait = NewPRGait;
}

float UPRMovementSystemComponent::GetSpeedWhenStopping() const
{
	return SpeedWhenStopping;
}

void UPRMovementSystemComponent::SetSpeedWhenStopping()
{
	if(bIsAccelerating)
	{
		bIsSetSpeedWhenStopping = false;
	}
	else if(!bIsSetSpeedWhenStopping)
	{
		SpeedWhenStopping = Speed;
		bIsSetSpeedWhenStopping = true;
	}
}

float UPRMovementSystemComponent::GetMinAnalogWalkSpeed() const
{
	return MinAnalogWalkSpeed;
}

float UPRMovementSystemComponent::GetSpeed() const
{
	return Speed;
}

float UPRMovementSystemComponent::GetWalkSpeed() const
{
	return WalkSpeed;
}

float UPRMovementSystemComponent::GetRunSpeed() const
{
	return RunSpeed;
}

float UPRMovementSystemComponent::GetSprintSpeed() const
{
	return SprintSpeed;
}

void UPRMovementSystemComponent::UpdateGravityControlInTheAir()
{
	if(bIsAerial)
	{
		return;
	}

	if(MovementState == EPRMovementState::MovementState_InAir)
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(GravityControlDelayTimerhandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(GravityControlDelayTimerhandle, FTimerDelegate::CreateLambda([&]()
			{
				PROwner->GetCharacterMovement()->GravityScale = IncreaseGravityScale;

				GetWorld()->GetTimerManager().ClearTimer(GravityControlDelayTimerhandle);
			}), GravityControlDelayTime, false);
		}
	}
	else
	{
		PROwner->GetCharacterMovement()->GravityScale = 1.0f;
		GetWorld()->GetTimerManager().ClearTimer(GravityControlDelayTimerhandle);
	}
}

void UPRMovementSystemComponent::ExecuteAerial(bool bFlag)
{
	bIsAerial = bFlag;

	if(bIsAerial)
	{
		bWasAerial = true;
		GetWorld()->GetTimerManager().ClearTimer(GravityControlDelayTimerhandle);
		PROwner->GetCharacterMovement()->StopMovementImmediately();
		PROwner->GetCharacterMovement()->GravityScale = 0.0f;
	}
	else
	{
		PROwner->GetCharacterMovement()->GravityScale = 1.0f;
	}
}

bool UPRMovementSystemComponent::WasAerial() const
{
	return bWasAerial;
}

void UPRMovementSystemComponent::SetWasAerial(bool bFlag)
{
	bWasAerial = bFlag;
}
