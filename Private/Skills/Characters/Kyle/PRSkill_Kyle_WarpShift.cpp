// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRSkill_Kyle_WarpShift.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UPRSkill_Kyle_WarpShift::UPRSkill_Kyle_WarpShift()
{
	bTickable = true;

	// WarpShift
	MoveLerpFloatCurve = nullptr;
	WarpShiftSpeedRatio = 1.0f;
	WarpDistance = 400.0f;
	DetectFloorDistance = 100.0f;
	CurrentOwnerLocation = FVector::ZeroVector;
	WarpShiftTarget = nullptr;
	WarpShiftLocation = FVector::ZeroVector;

	// Debug
	bDebugFlag = false;

	// Effect
	RibbonEffect = nullptr;
	SocketName = TEXT("root");
	LocationOffset = FVector::ZeroVector;
	RotationOffset = FRotator::ZeroRotator;
	bOverrideEffectColor = false;
	EffectColor = FLinearColor::White;
	EffectSize = 1.0f;
	
	// PRAnimMontage
	CastingWarpShiftPRAnimMontageID = 1114;
	CastingWarpShiftPRAnimMontage = FPRAnimMontage();
	InGroundWarpShiftFinishPRAnimMontageID = 1115;
	InGroundWarpShiftFinishPRAnimMontage = FPRAnimMontage();
	InAerialWarpShiftFinishPRAnimMontageID = 1116;
	InAerialWarpShiftFinishPRAnimMontage = FPRAnimMontage();
}

void UPRSkill_Kyle_WarpShift::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveLerpTimeline.TickTimeline(DeltaTime);
}

void UPRSkill_Kyle_WarpShift::InitializeSkill_Implementation()
{
	Super::InitializeSkill_Implementation();

	// 데이터 테이블에서 스킬이 사용하는 PRAnimMontage를 가져옵니다.
	// WarpShiftPRAnimMontage를 데이터 테이블에서 받아옵니다.
	if(IsValid(GetSkillOwner()) == true
		&& (CastingWarpShiftPRAnimMontage == FPRAnimMontage()
			|| InGroundWarpShiftFinishPRAnimMontage == FPRAnimMontage()
			|| InAerialWarpShiftFinishPRAnimMontage == FPRAnimMontage()) == true)
	{
		CastingWarpShiftPRAnimMontage  = FPRAnimMontage(GetSkillOwner()->GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByID(CastingWarpShiftPRAnimMontageID));
		InGroundWarpShiftFinishPRAnimMontage = FPRAnimMontage(GetSkillOwner()->GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByID(InGroundWarpShiftFinishPRAnimMontageID));
		InAerialWarpShiftFinishPRAnimMontage = FPRAnimMontage(GetSkillOwner()->GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByID(InAerialWarpShiftFinishPRAnimMontageID));
	}

	CurrentOwnerLocation = FVector::ZeroVector;
	WarpShiftTarget = nullptr;
	
	// Timeline을 초기화합니다.
	InitializeMoveLerpTimeline();
}

bool UPRSkill_Kyle_WarpShift::ActivateSkill_Implementation()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsCanActivateSkill() == true && PRPlayerCharacter != nullptr)
	{
		bActivateSkill = true;
		GetSkillOwner()->InitializePlayNormalAttackIndex();

		// 재사용 대기시간을 실행합니다.
		if(IsCooldown() == false)
		{
			ActivateCooldown();
		}

		// AfterImage를 실행합니다.
		PRPlayerCharacter->ActivateAfterImage();
		
		// Target을 락온 중일 경우
		if(PRPlayerCharacter->GetTargetingSystem()->IsActivateLockOnTarget() == true)
		{
			WarpShiftTarget = PRPlayerCharacter->GetTargetingSystem()->GetLockedOnTarget();
			// 캐릭터를 Target을 향해 회전시킵니다. Target이 없을 경우 캐릭터의 전방을 바라봅니다.
			PRPlayerCharacter->RotationAutoTargetDirection();
			// TargetSystem의 동적 CameraLock을 활성화합니다.
			PRPlayerCharacter->GetTargetingSystem()->SetActivateDynamicCameraLock(true);
		}
		else
		{
			WarpShiftTarget = nullptr;
		}

		// 현재 캐릭터의 위치를 저장합니다.
		CurrentOwnerLocation = GetSkillOwner()->GetActorLocation();

		// WarpShift할 위치를 저장합니다.
		WarpShiftLocation = CalculateWarpShiftLocation();

		// Actionable을 설정합니다.
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Dodge, false);

		GetSkillOwner()->GetAnimSystem()->StopPRAnimMontage();

		// 캐릭터가 공중에서 실행할 경우 Aerial을 실행합니다. 
		if(GetSkillOwner()->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
		{
			GetSkillOwner()->GetMovementSystem()->ActivateAerial(true);
		}

		// SkillOwner의 Collision을 비활성화합니다. 
		SetEnableCollision(false);
		
		GetSkillOwner()->PlayAnimMontage(CastingWarpShiftPRAnimMontage.AnimMontage);

		// SkillOwner의 Mesh를 비활성화합니다.
		GetSkillOwner()->GetMesh()->SetVisibility(false);

		// RibbonEffect를 활성화합니다.
		ActivateRibbonEffect();
		
		// 타임라인이 재생속도를 변경하여 WarpShift의 이동속도에 변화를 줍니다.
		MoveLerpTimeline.SetPlayRate(WarpShiftSpeedRatio);
		MoveLerpTimeline.PlayFromStart();

		return true;
	}
	
	return false;
}

bool UPRSkill_Kyle_WarpShift::IsCanActivateSkill_Implementation() const
{
	return Super::IsCanActivateSkill_Implementation()
			&& GetSkillOwner()->GetStateSystem()->IsActionable(EPRAction::Action_Attack) == true;
}

void UPRSkill_Kyle_WarpShift::InitializeMoveLerpTimeline()
{
	float CurveMinTime = 0.0f;
	float CurveMaxTime = 0.0f;

	if(MoveLerpTimelineProgress.IsBound() == false && MoveLerpFloatCurve != nullptr)
	{
		// Callback 함수에서 사용할 함수를 바인딩합니다.
		MoveLerpTimelineProgress.BindUFunction(this, FName("MoveLerp"));
		MoveLerpTimelineFinish.BindUFunction(this, FName("MoveLerpFinish"));
		
		// Timeline에 Curve를 추가합니다.
		MoveLerpTimeline.AddInterpFloat(MoveLerpFloatCurve, MoveLerpTimelineProgress, NAME_None, TEXT("MoveLerp"));
		// Timeline에 Finish함수를 추가합니다.
		MoveLerpTimeline.SetTimelineFinishedFunc(MoveLerpTimelineFinish);
		MoveLerpFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
		MoveLerpTimeline.SetPlayRate(1.0f);
	}
		
	MoveLerpTimeline.SetTimelineLength(CurveMaxTime);
}

void UPRSkill_Kyle_WarpShift::MoveLerp(float Value)
{
	if(MoveLerpFloatCurve != nullptr)
	{
		FVector MoveLocation = FVector::ZeroVector;
		FVector NewMoveLocation = FVector::ZeroVector;
		
		if(WarpShiftTarget != nullptr)
		{
			// Target이 있을 경우
			MoveLocation = UKismetMathLibrary::VEase(CurrentOwnerLocation, WarpShiftLocation, Value, EEasingFunc::ExpoIn);
			
			GetSkillOwner()->SetActorLocation(MoveLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			// Target이 없을 경우
			MoveLocation = FMath::Lerp(CurrentOwnerLocation, GetSkillOwner()->GetActorForwardVector() * WarpDistance + CurrentOwnerLocation, Value);
			NewMoveLocation = FVector(MoveLocation.X, MoveLocation.Y, GetSkillOwner()->GetActorLocation().Z);

			GetSkillOwner()->SetActorLocation(NewMoveLocation, true);
		}
	}
}

void UPRSkill_Kyle_WarpShift::MoveLerpFinish()
{
	// SkillOwner의 Collision을 활성화합니다.
	SetEnableCollision(true);

	// RibbonEffect를 비활성화합니다.
	DeactivateRibbonEffect();

	// SkillOwner의 Mesh를 활성화합니다.
	GetSkillOwner()->GetMesh()->SetVisibility(true);
	
	// Actionable을 설정합니다.
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Dodge, false);
	
	// WarpShift 후 캐릭터의 위치에 따라 AnimMontage를 재생합니다.
	if(CalculateMovementStateWhenFinishWarpShift() == EPRMovementState::MovementState_Grounded)
	{
		GetSkillOwner()->PlayAnimMontage(InGroundWarpShiftFinishPRAnimMontage.AnimMontage);
	}
	else
	{
		GetSkillOwner()->PlayAnimMontage(InAerialWarpShiftFinishPRAnimMontage.AnimMontage);
	}

	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(PRPlayerCharacter != nullptr)
	{
		// AfterImage를 실행합니다.
		PRPlayerCharacter->ActivateAfterImage();
	}
}

FVector UPRSkill_Kyle_WarpShift::CalculateWarpShiftLocation() const
{
	if(IsValid(GetSkillOwner()) == true && IsValid(WarpShiftTarget) == true)
	{
		FVector StartLocation = GetSkillOwner()->GetActorLocation();
		FVector TargetLocation = WarpShiftTarget->GetActorLocation();
		FVector MoveDirection = TargetLocation - StartLocation;
		float MoveDistance = MoveDirection.Size();
		
		if(MoveDistance > KINDA_SMALL_NUMBER)
		{
			FVector NormalizedMoveDirection = MoveDirection.GetSafeNormal();
			FVector EndLocation = TargetLocation;

			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Emplace(GetSkillOwner());
			
			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
			if(bDebugFlag)
			{
				DebugType = EDrawDebugTrace::ForDuration;
			}
				
			// ECollisionChannel::ECC_GameTraceChannel8는 TraceChannels의 LockOnTargeting을 나타냅니다.
			bool bIsHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation, GetSkillOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius(),
																	GetSkillOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel8),
																	false, ActorsToIgnore, DebugType, HitResult, true);
			if(bIsHit)
			{
				// 충돌이 발생한 경우 충돌 지점을 보정하여 캐릭터의 위치를 조정합니다.
				FVector AdjustedLocation = HitResult.Location - NormalizedMoveDirection * GetSkillOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius() / 2.0f;
				AdjustedLocation = FVector(AdjustedLocation.X, AdjustedLocation.Y, TargetLocation.Z);
					
				// 충돌 보정 후 위치를 반환합니다.
				return AdjustedLocation;
			}
			else
			{
				// 충돌이 발생하지 않은 경우 정상적인 위치를 반환합니다.
				return TargetLocation;
			}
		}
	}

	// 전방으로 WarpShift합니다.
	return GetSkillOwner()->GetActorForwardVector() * WarpDistance + CurrentOwnerLocation;
}

EPRMovementState UPRSkill_Kyle_WarpShift::CalculateMovementStateWhenFinishWarpShift()
{
	if(IsValid(GetSkillOwner()) == true)
	{
		FVector StartLocation = WarpShiftLocation;
		FVector EndLocation = WarpShiftLocation - FVector(0.0f, 0.0f, GetSkillOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
								- FVector::UpVector * DetectFloorDistance;

		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Emplace(GetSkillOwner());
	
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bDebugFlag)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}
	
		bool bIsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility),
															false, ActorsToIgnore, DebugType, HitResult, true);
		if(!bIsHit)
		{
			return EPRMovementState::MovementState_InAir;
		}
	}

	return EPRMovementState::MovementState_Grounded;
}

void UPRSkill_Kyle_WarpShift::SetEnableCollision(bool bSetAble)
{
	if(IsValid(GetSkillOwner()) == true)
	{
		if(bSetAble)
		{
			// Collision을 활성화합니다.
			GetSkillOwner()->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetSkillOwner()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		else
		{
			// Collision을 비활성화합니다.
			GetSkillOwner()->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetSkillOwner()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

#pragma region Effect
void UPRSkill_Kyle_WarpShift::ActivateRibbonEffect()
{
	if(RibbonEffectComponent == nullptr)
	{
		RibbonEffectComponent = SpawnRibbonEffect();
	}
	
	RibbonEffectComponent->Activate(true);
}

void UPRSkill_Kyle_WarpShift::DeactivateRibbonEffect()
{
	RibbonEffectComponent->Deactivate();
}

UNiagaraComponent* UPRSkill_Kyle_WarpShift::SpawnRibbonEffect() const
{
	if(IsValid(GetSkillOwner()) == true && RibbonEffect != nullptr)
	{
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(RibbonEffect, GetSkillOwner()->GetMesh(), SocketName,
																								LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, false);

		if(bOverrideEffectColor)
		{
			SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", EffectColor);
		}
		else
		{
			SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", GetSkillOwner()->GetSignatureEffectColor());
		}

		SpawnNiagaraComponent->SetVariableFloat("EffectSize", EffectSize);

		return SpawnNiagaraComponent;
	}

	return nullptr;
}
#pragma endregion 
