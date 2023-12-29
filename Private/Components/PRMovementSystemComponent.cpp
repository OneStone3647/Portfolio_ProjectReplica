// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "AnimInstances/PRBaseAnimInstance.h"
#include "Animation/AnimCurveCompressionCodec_UniformIndexable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UPRMovementSystemComponent::UPRMovementSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// MovementInfo
	MovementState = EPRMovementState::MovementState_None;
	PreviousMovementState = EPRMovementState::MovementState_None;
	AllowedGait = EPRGait::Gait_Running;
	PreviousAllowedGait = EPRGait::Gait_None;
	Gait = EPRGait::Gait_Walking;
	RotationMode = EPRRotationMode::RotationMode_VelocityDirection;
	CurrentSpeed = 0.0f;
	CurrentSpeed2D = 0.0f;
	bInitializeSpeedWhenStop = true;
	SpeedWhenStop = 0.0f;
	bInitializeSpeedWhenJump = true;
	SpeedWhenJump = 0.0f;
	bIsMoving = false;

	// Walk
	WalkSpeed = 220.0f;
	WalkAcceleration = 4000.0f;
	// WalkBrakingDeceleration = 1500.0f;
	// WalkBrakingFrictionFactor = 5.0f;
	WalkBrakingDeceleration = 2048.0f;
	WalkBrakingFrictionFactor = 1.0f;

	// Run
	RunSpeed = 400.0f;
	RunAcceleration = 2000.0f;
	RunBrakingDeceleration = 40.0f;
	RunBrakingFrictionFactor = 0.7f;

	// Sprint
	SprintSpeed = 800.0f;
	SprintAcceleration = 750.0f;
	SprintBrakingDeceleration = 80.0f;
	SprintBrakingFrictionFactor = 0.7f;

	// GravityControl
	DefaultGravity = 2.0f;
	// GravityControlDelayTime = 0.5f;
	// IncreaseGravityScale = 2.0f;
	bActivateAerial = false;
}

void UPRMovementSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// MovementInfo
	InitializeMovementInfo();
	InitializeMaxWalkSpeedTimeline();
}


void UPRMovementSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// MovementInfo
	MaxWalkSpeedTimeline.TickTimeline(DeltaTime);
	UpdateMovementInfo();

	// GravityControl
	// UpdateGravityControlInTheAir();
}

#pragma region DistanceMatching
bool UPRMovementSystemComponent::PredictStopLocation(FVector& OutStopLocation, const FVector& CurrentLocation, const FVector& Velocity, const FVector& Acceleration, float Friction, float BrakingDeceleration,	const float TimeStep, const int32 MaxSimulationIterations)
{
	// 최소 틱 시간보다 작으면 false를 반환합니다.
	const float MinTickTime = 1e-6;
	if(TimeStep < MinTickTime)
	{
		return false;
	}

	const bool bZeroAcceleration = Acceleration.IsZero();

	// 현재 가속도와 속도의 내적이 0보다 크면(두 벡터같의 각도가 90도 보다 작으면) false를 반환합니다. 
	if((Acceleration | Velocity) > 0.0f)
	{
		return false;
	}

	BrakingDeceleration = FMath::Max(BrakingDeceleration, 0.0f);
	Friction = FMath::Max(Friction, 0.0f);
	const bool bZeroFriction = (Friction == 0.0f);
	const bool bZeroBraking = (BrakingDeceleration == 0.0f);

	// 가속도와 마찰이 0일 경우 false를 반환합니다.
	if(bZeroAcceleration && bZeroFriction)
	{
		return false;
	}

	FVector LastVelocity = bZeroAcceleration ? Velocity : Velocity.ProjectOnToNormal(Acceleration.GetSafeNormal());
	LastVelocity.Z = 0.0f;

	FVector LastLocation = CurrentLocation;

	int32 Iterations = 0;
	while(Iterations < MaxSimulationIterations)
	{
		Iterations++;

		const FVector OldVelocity = LastVelocity;

		// 가속이 없거나 최대 속도를 초과하여 속도를 줄여야 하는 경우에만 제동을 적용합니다.
		if(bZeroAcceleration)
		{
			// 더 낮은 프레임 속도에서 합리적으로 일관된 결과를 얻기위해 제동을 세분화합니다.
			// (네트워킹이 있는 패킷 손실 상황에 중요합니다.)
			float RemainingTime = TimeStep;
			const float MaxTimeStep = (1.0f / 33.0f);

			// 정지할 때 까지 감속합니다.
			const FVector ReverseAcceleration = (bZeroBraking ? FVector::ZeroVector : (-BrakingDeceleration * LastVelocity.GetSafeNormal()));
			while(RemainingTime >= MinTickTime)
			{
				// 제로 마찰은 일정한 감속을 사용하므로 반복할 필요가 없습니다.
				const float DecelerateTime = ((RemainingTime > MaxTimeStep && !bZeroFriction) ? FMath::Min(MaxTimeStep, RemainingTime * 0.5f) : RemainingTime);
				RemainingTime -= DecelerateTime;

				// 마찰과 제동을 적용합니다.
				LastVelocity = LastVelocity + ((-Friction) * LastVelocity + ReverseAcceleration) * DecelerateTime;

				// 방향을 바꾸지 않습니다.
				if((LastVelocity | OldVelocity) <= 0.0f)
				{
					LastVelocity = FVector::ZeroVector;
					break;
				}
			}

			// 0에 가깝거나 최소 임계값 미만인 경우 0으로 고정하고 제동합니다.
			const float VelocitySizeSquared = LastVelocity.SizeSquared();
			if(VelocitySizeSquared <= 1.0f || (!bZeroBraking && VelocitySizeSquared <= FMath::Square(10)))
			{
				LastVelocity = FVector::ZeroVector;
			}
		}
		else
		{
			FVector TotalAcceleration = Acceleration;
			TotalAcceleration.Z = 0.0f;

			// 마찰은 방향을 바꾸는 능력에 영향을 줍니다.
			// 이것은 경로 추적이 아닌 입력 가속에 대해서만 수행됩니다.
			const FVector AccelerationDriection = TotalAcceleration.GetSafeNormal();
			const float VelocitySize = LastVelocity.Size();
			TotalAcceleration += -(LastVelocity - AccelerationDriection * VelocitySize) * Friction;
			// 가속을 적용합니다.
			LastVelocity += TotalAcceleration * TimeStep;
		}

		LastLocation += LastVelocity * TimeStep;

		// 0에 가깝거나 최소 입계값 미만이 경우 0으로 고정하고 제동합니다.
		const float VelocitySizeSquared = LastVelocity.SizeSquared();
		if(VelocitySizeSquared <= 1.0f || (LastVelocity | OldVelocity) <= 0.0f)
		{
			OutStopLocation = LastLocation;
			return true;
		}
	}

	return false;
}

float UPRMovementSystemComponent::GetCurveTime(const UAnimSequence* AnimationSequence, const FName CurveName, const float CurveValue)
{
	if(AnimationSequence == nullptr)
	{
		return 0.0f;
	}

	// Curve의 SmartName을 가져옵니다.
	FSmartName CurveSmartName;
	AnimationSequence->GetSkeleton()->GetSmartNameByName(USkeleton::AnimCurveMappingName, CurveName, CurveSmartName);

    // Curve의 시간과 값에 해당하는 버퍼링된 액세스를 생성합니다.
	const FAnimCurveBufferAccess CurveBuffer = FAnimCurveBufferAccess(AnimationSequence, CurveSmartName.UID);
	const int NumSamples = static_cast<int>(CurveBuffer.GetNumSamples());
	const int LastIndex = NumSamples - 1;
	
	if(NumSamples < 2)
	{
		return 0.0f;
	}

	if(CurveValue <= CurveBuffer.GetValue(0))
	{
		return CurveBuffer.GetTime(0);
	}
	if(CurveValue >= CurveBuffer.GetValue(LastIndex))
	{
		return CurveBuffer.GetTime(LastIndex);
	}

	// 이진 탐색
	int32 NextIndex = 1;
	int32 Count = LastIndex - NextIndex;
	while(Count > 0)
	{
		const int32 Step = Count / 2;
		const int32 Middle = NextIndex + Step;

		if(CurveValue > CurveBuffer.GetValue(Middle))
		{
			NextIndex = Middle + 1;
			Count -= Step + 1;
		}
		else
		{
			Count = Step;
		}
	}

	const int32 PrevIndex = NextIndex - 1;
	const float PrevCurveValue = CurveBuffer.GetValue(PrevIndex);
	const float NextCurveValue = CurveBuffer.GetValue(NextIndex);
	const float PrevCurveTime = CurveBuffer.GetTime(PrevIndex);
	const float NextCurveTime = CurveBuffer.GetTime(NextIndex);

	// Curve에서 가장 가까운 두 점으로 시간을 반환합니다.
	const float Diff = NextCurveValue - PrevCurveValue;
	const float Alpha = !FMath::IsNearlyZero(Diff) ? (CurveValue - PrevCurveValue) / Diff : 0.0f;

	return FMath::Lerp(PrevCurveTime, NextCurveTime, Alpha);
}
#pragma endregion 

#pragma region MovementInfo
void UPRMovementSystemComponent::OnCharacterMovementModeChanged(EMovementMode NewMovementMode)
{
	switch(NewMovementMode)
	{
	case EMovementMode::MOVE_Walking:
	case EMovementMode::MOVE_NavWalking:
		SetMovementState(EPRMovementState::MovementState_Grounded);
		break;
	case EMovementMode::MOVE_Falling:
		SetMovementState(EPRMovementState::MovementState_InAir);
		break;
	default:
		break;
	}
}

bool UPRMovementSystemComponent::IsEqualMovementState(const EPRMovementState& NewMovementState) const
{
	return MovementState == NewMovementState;
}

bool UPRMovementSystemComponent::IsEqualAllowedGait(const EPRGait& NewAllowedGait) const
{
	return AllowedGait == NewAllowedGait;
}

void UPRMovementSystemComponent::SetAllowedGait(EPRGait NewAllowedGait)
{
	if(AllowedGait == NewAllowedGait || IsValid(GetPROwner()) == false)
	{
		return;
	}

	PreviousAllowedGait = AllowedGait;
	AllowedGait = NewAllowedGait;
	switch(AllowedGait)
	{
	case EPRGait::Gait_Walking:
		GetPROwner()->GetCharacterMovement()->MinAnalogWalkSpeed = GetMaxWalkSpeed(EPRGait::Gait_Walking);
		break;
	case EPRGait::Gait_Sprinting:
		GetPROwner()->GetCharacterMovement()->MinAnalogWalkSpeed = GetMaxWalkSpeed(EPRGait::Gait_Sprinting);
		break;
	case EPRGait::Gait_Running:
	default:
		GetPROwner()->GetCharacterMovement()->MinAnalogWalkSpeed = GetMaxWalkSpeed(EPRGait::Gait_Running);
		break;
	}

	if(PreviousAllowedGait != AllowedGait || MaxWalkSpeedTimeline.IsPlaying() == false)
	{
		MaxWalkSpeedTimeline.PlayFromStart();
	}
}

bool UPRMovementSystemComponent::IsEqualGait(const EPRGait& NewGait) const
{
	return Gait == NewGait;
}

void UPRMovementSystemComponent::UpdateGait(float NewSpeed)
{
	if(!IsValid(GetPROwner()))
	{
		return;
	}
	
	if(NewSpeed > RunSpeed + 5.0f)
	{
		Gait = EPRGait::Gait_Sprinting;
		GetPROwner()->GetCharacterMovement()->MaxAcceleration = SprintAcceleration;
	}
	else if(NewSpeed > WalkSpeed + 5.0f)
	{
		Gait = EPRGait::Gait_Running;
		GetPROwner()->GetCharacterMovement()->MaxAcceleration = RunAcceleration;
	}
	else
	{
		Gait = EPRGait::Gait_Walking;
		GetPROwner()->GetCharacterMovement()->MaxAcceleration = WalkAcceleration;
	}
}

void UPRMovementSystemComponent::SetRotationMode(EPRRotationMode NewRotationMode)
{
	if(!IsValid(GetPROwner()))
	{
		return;
	}
	
	RotationMode = NewRotationMode;
	switch(RotationMode)
	{
	case EPRRotationMode::RotationMode_VelocityDirection:
		GetPROwner()->GetCharacterMovement()->bOrientRotationToMovement = true;
		GetPROwner()->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EPRRotationMode::RotationMode_LookingDirection:
		GetPROwner()->GetCharacterMovement()->bOrientRotationToMovement = false;
		GetPROwner()->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		break;
	default:
		break;
	}
}

float UPRMovementSystemComponent::GetMaxWalkSpeed(EPRGait NewGait)
{
	switch(NewGait)
	{
	case EPRGait::Gait_Walking:
		return WalkSpeed;
	case EPRGait::Gait_Running:
		return RunSpeed;
	case EPRGait::Gait_Sprinting:
		return SprintSpeed;
	default:
		break;
	}

	return RunSpeed;
}

bool UPRMovementSystemComponent::IsMoving() const
{
	return bIsMoving;
}

bool UPRMovementSystemComponent::IsAccelerating() const
{
	if(IsValid(GetPROwner()) == true)
	{
		return CurrentAcceleration.Size() > KINDA_SMALL_NUMBER;
	}

	return false;
}

void UPRMovementSystemComponent::InitializeMovementInfo()
{
	if(IsValid(GetPROwner()) == true)
	{
		GetPROwner()->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void UPRMovementSystemComponent::UpdateMovementInfo()
{
	if(IsValid(GetPROwner()) == true)
	{
		CurrentSpeed = GetPROwner()->GetCharacterMovement()->Velocity.Size();
		CurrentSpeed2D = GetPROwner()->GetVelocity().Size2D();
		UpdateGait(CurrentSpeed);
		// bIsMoving
		if(GetPROwner()->GetCharacterMovement()->GetLastUpdateVelocity().Size() != 0.0f)
		{
			bIsMoving = true;
		}
		else
		{
			bIsMoving = false;
		}
		CurrentAcceleration = GetPROwner()->GetCharacterMovement()->GetCurrentAcceleration();

		SetSpeedWhenStop(SpeedWhenStop);
		SetSpeedWhenJump(SpeedWhenJump);
		SetBrakingInfo(SpeedWhenStop);
	}
}

void UPRMovementSystemComponent::InitializeMaxWalkSpeedTimeline()
{
	// Curve가 존재할 경우 Timeline을 초기화합니다.
	if(ChangeMaxWalkSpeedAlphaCurveFloat)
	{
		// Curve가 사용할 Callback 함수입니다.
		FOnTimelineFloat TimelineProgress;

		// Callback 함수에서 사용할 함수를 바인딩합니다.
		TimelineProgress.BindUFunction(this, FName("ChangeMaxWalkSpeed"));

		// Timeline에 Curve를 추가합니다.
		MaxWalkSpeedTimeline.AddInterpFloat(ChangeMaxWalkSpeedAlphaCurveFloat, TimelineProgress, NAME_None, TEXT("ChangeMaxWalkSpeed"));
	}
}

void UPRMovementSystemComponent::ChangeMaxWalkSpeed(float Value)
{
	float AllowedMaxWalkSpeed;
	
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
	default:
		AllowedMaxWalkSpeed = RunSpeed;
	}

	float NewMaxWalkSpeed = UKismetMathLibrary::Lerp(GetMaxWalkSpeed(PreviousAllowedGait), AllowedMaxWalkSpeed, Value);
	GetPROwner()->GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;

	if(GetPROwner()->GetCharacterMovement()->MaxWalkSpeed == AllowedMaxWalkSpeed)
	{
		MaxWalkSpeedTimeline.Stop();
	}
}

void UPRMovementSystemComponent::SetSpeedWhenStop(float& NewSpeedWhenStop)
{
	if(CurrentAcceleration.Size())
	{
		bInitializeSpeedWhenStop = true;
		NewSpeedWhenStop = 0.0f;
	}
	else if(bInitializeSpeedWhenStop)
	{
		bInitializeSpeedWhenStop = false;
		// 가속이 끝나고 SpeedWhenStop을 초기화하지 않았을 때 CurrentSpeed를 SpeedWhenStop에 저장합니다.
		NewSpeedWhenStop = CurrentSpeed;
	}
}

void UPRMovementSystemComponent::SetSpeedWhenJump(float& NewSpeedWhenJump)
{
	if(bInitializeSpeedWhenJump == true && IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(SpeedWhenJumpDelayTimerHandle) == true)
		{
			GetWorld()->GetTimerManager().ClearTimer(SpeedWhenJumpDelayTimerHandle);
		}

		NewSpeedWhenJump = CurrentSpeed2D;
		bInitializeSpeedWhenJump = false;
	}
	else if(bInitializeSpeedWhenJump == false && IsEqualMovementState(EPRMovementState::MovementState_InAir) == false)
	{
		bInitializeSpeedWhenJump = true;
		
		if(GetWorld()->GetTimerManager().IsTimerActive(SpeedWhenJumpDelayTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(SpeedWhenJumpDelayTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				NewSpeedWhenJump = 0.0f;

				GetWorld()->GetTimerManager().ClearTimer(SpeedWhenJumpDelayTimerHandle);
			}), 5.0f, false);
		}
	}
}

void UPRMovementSystemComponent::SetBrakingInfo(float NewSpeedWhenStop)
{
	if(IsValid(GetPROwner()) == false)
	{
		return;
	}

	if(NewSpeedWhenStop < WalkSpeed)
	{
		GetPROwner()->GetCharacterMovement()->BrakingDecelerationWalking = WalkBrakingDeceleration;
		GetPROwner()->GetCharacterMovement()->BrakingFrictionFactor = WalkBrakingFrictionFactor;
	}
	else if(NewSpeedWhenStop < RunSpeed)
	{
		// PROwner->GetCharacterMovement()->BrakingDecelerationWalking = RunBrakingDeceleration;
		// PROwner->GetCharacterMovement()->BrakingFrictionFactor = RunBrakingFrictionFactor;

		float NewBrakingDecelerationWalking = FMath::Lerp(WalkBrakingDeceleration, RunBrakingDeceleration, NewSpeedWhenStop / RunSpeed);
		float NewBrakingFrictionFactor = FMath::Lerp(WalkBrakingFrictionFactor, RunBrakingFrictionFactor, NewSpeedWhenStop / RunSpeed);

		GetPROwner()->GetCharacterMovement()->BrakingDecelerationWalking = NewBrakingDecelerationWalking;
		GetPROwner()->GetCharacterMovement()->BrakingFrictionFactor = NewBrakingFrictionFactor;
	}
	else
	{
		GetPROwner()->GetCharacterMovement()->BrakingDecelerationWalking = SprintBrakingDeceleration;
		GetPROwner()->GetCharacterMovement()->BrakingFrictionFactor = SprintBrakingFrictionFactor;
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

EPRGait UPRMovementSystemComponent::GetAllowedGait() const
{
	return AllowedGait;
}

EPRGait UPRMovementSystemComponent::GetGait() const
{
	return Gait;
}

float UPRMovementSystemComponent::GetCurrentSpeed() const
{
	return CurrentSpeed;
}

float UPRMovementSystemComponent::GetSpeedWhenStop() const
{
	return SpeedWhenStop;
}

void UPRMovementSystemComponent::SetInitializeSpeedWhenJump(bool bFlag)
{
	bInitializeSpeedWhenJump = bFlag;
}

float UPRMovementSystemComponent::GetSpeedWhenJump() const
{
	return SpeedWhenJump;
}
#pragma endregion 

#pragma region GravityControl
void UPRMovementSystemComponent::ActivateAerial(bool bNewActiveAerial)
{
	bActivateAerial = bNewActiveAerial;
	if(bActivateAerial)
	{
		DefaultGravity = GetPROwner()->GetCharacterMovement()->GravityScale;
		GetPROwner()->GetCharacterMovement()->StopMovementImmediately();
		GetPROwner()->GetCharacterMovement()->GravityScale = 0.0f;
	}
	else
	{
		GetPROwner()->GetCharacterMovement()->GravityScale = DefaultGravity;
	}
}

// void UPRMovementSystemComponent::UpdateGravityControlInTheAir()
// {
// 	if(bActivateAerial)
// 	{
// 		return;
// 	}
//
// 	if(IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
// 	{
// 		if(GetWorld()->GetTimerManager().IsTimerActive(GravityControlDelayTimerHandle) == false)
// 		{
// 			GetWorld()->GetTimerManager().SetTimer(GravityControlDelayTimerHandle, FTimerDelegate::CreateLambda([&]()
// 			{
// 				PROwner->GetCharacterMovement()->GravityScale = IncreaseGravityScale;
//
// 				GetWorld()->GetTimerManager().ClearTimer(GravityControlDelayTimerHandle);
// 			}), GravityControlDelayTime, false);
// 		}
// 	}
// 	else
// 	{
// 		PROwner->GetCharacterMovement()->GravityScale = 1.0f;
// 		if(GetWorld()->GetTimerManager().IsTimerActive(GravityControlDelayTimerHandle) == true)
// 		{
// 			GetWorld()->GetTimerManager().ClearTimer(GravityControlDelayTimerHandle);
// 		}
// 	}
// }
#pragma endregion

