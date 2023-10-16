// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/PRBaseSkill.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Objects/PRPooledObject.h"

UPRBaseSkill::UPRBaseSkill()
{
	// TickableGameObject
	bTickable = false;
	bTickableWhenPaused = false;
	
	// BaseSkill
	bActivateSkill = false;
	
	// SkillInfo
	SkillInfo = FPRSkillInfo();
	ActivateableCount = -1;
	SkillOwner = nullptr;

	// DurationEffect
	bActivateDurationEffect = false;
	DurationEffectRemaining = 0.0f;
	DurationEffectElapsed = 0.0f;
}

void UPRBaseSkill::Tick(float DeltaTime)
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		UpdateDurationEffect(DeltaTime);
	}
}

bool UPRBaseSkill::IsTickable() const
{
	return bTickable;
}

bool UPRBaseSkill::IsTickableInEditor() const
{
	return bTickable;
}

bool UPRBaseSkill::IsTickableWhenPaused() const
{
	return bTickableWhenPaused;
}

TStatId UPRBaseSkill::GetStatId() const
{
	return TStatId();
}

UWorld* UPRBaseSkill::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UPRBaseSkill::InitializeSkill_Implementation()
{
	if(IsValid(GetSkillOwner()) == true)
	{
		for(FPRPooledObjectInfo ObjectInfo : SkillInfo.ObjectInfos)
		{
			// 스킬에서 사용하는 Object의 정보가 있을 경우 Object의 정보를 ObjectPoolSystem에 전달합니다.
			if(ObjectInfo.PooledObjectClass != nullptr
				&& GetSkillOwner()->GetObjectPoolSystem()->IsCreatePooledObject(ObjectInfo.ObjectName) == false)
			{
				GetSkillOwner()->GetObjectPoolSystem()->AddPooledObjectInfo(ObjectInfo);
			}
		}
	}
}

bool UPRBaseSkill::ActivateSkill_Implementation()
{
	if(IsCanActivateSkill() == true)
	{
		bActivateSkill = true;
		GetSkillOwner()->InitializePlayNormalAttackIndex();
		ActivateDuration();
		ActivateCooldown();
		
		return true;
	}

	return false;
}

bool UPRBaseSkill::IsActivateSkill() const
{
	return bActivateSkill;
}

bool UPRBaseSkill::IsCanActivateSkill_Implementation() const
{
	return IsValid(SkillOwner) == true
			&& IsCooldown() == false
			&& IsCanActivatableType() == true;
}

bool UPRBaseSkill::IsCanActivatableType() const
{
	if(IsValid(SkillOwner) == true)
	{
		switch(SkillInfo.ActivatableType)
		{
		case EPRSkillActivatableType::SkillActivatableType_Ground:
			if(SkillOwner->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_Grounded) == true)
			{
				return true;
			}
			break;
		case EPRSkillActivatableType::SkillActivatableType_InAir:
			if(SkillOwner->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
			{
				return true;
			}
			break;
		case EPRSkillActivatableType::SkillActivatableType_All:
		default:
			return true;
		}
	}

	return false;
}

void UPRBaseSkill::SetActivateSkill(bool bNewActivateSkill)
{
	bActivateSkill = bNewActivateSkill;

	// 스킬이 비활성화 될 경우 지속효과를 해제합니다.
	if(!bNewActivateSkill && GetWorld()->GetTimerManager().IsTimerActive(DurationTimerHandle) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
		EndDurationEffect();
	}
}

void UPRBaseSkill::SetSkillInfo(FPRSkillInfo NewSkillInfo)
{
	SkillInfo = NewSkillInfo;
}

void UPRBaseSkill::SetSkillOwner(APRBaseCharacter* NewSkillOwner)
{
	SkillOwner = NewSkillOwner;
}

void UPRBaseSkill::SetActivateableCount(int32 NewActivateableCount)
{
	ActivateableCount = NewActivateableCount;
}
#pragma region Cooldown
void UPRBaseSkill::ActivateCooldown_Implementation()
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
}

bool UPRBaseSkill::IsCooldown() const
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
	return false;
}

float UPRBaseSkill::GetSkillCooldownRemaining() const
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
	return 0.0f;
}

float UPRBaseSkill::GetSkillCooldownRemainingRatio() const
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
	return 0.0f;
}

float UPRBaseSkill::GetSkillCooldownElapsed() const
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
	return 0.0f;
}

float UPRBaseSkill::GetSkillCooldownElapsedRatio() const
{
	// PRCooldownSkill 클래스에서 Override하여 사용합니다.
	return 0.0f;
}
#pragma endregion 

#pragma region Duration
void UPRBaseSkill::ActivateDuration()
{
	// 지속효과를 실행합니다.
	DurationEffect();

	// TimeStop에 영향을 받을 경우
	if(SkillInfo.bIgnoreTimeStop == false)
	{
		// 지속시간이 지난 후 효과를 종료합니다.
		GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			EndDurationEffect();
		
			GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
		}), SkillInfo.Duration, false);
	}
}

float UPRBaseSkill::GetRemainingDurationEffect() const
{
	// TimeStop에 영향을 받지 않을 경우
	if(SkillInfo.bIgnoreTimeStop)
	{
		return DurationEffectRemaining;
	}
	
	return GetWorld()->GetTimerManager().GetTimerRemaining(DurationTimerHandle);
}

void UPRBaseSkill::DurationEffect()
{
	// TimeStop에 영향을 받지 않을 경우
	if(SkillInfo.bIgnoreTimeStop)
	{
		bActivateDurationEffect = true;
	}
	
	if(DurationSkillDelegate.IsBound() == true)
	{
		DurationSkillDelegate.Broadcast();
	}
}

void UPRBaseSkill::EndDurationEffect()
{
	// TimeStop에 영향을 받지 않을 경우
	if(SkillInfo.bIgnoreTimeStop)
	{
		bActivateDurationEffect = false;
		DurationEffectElapsed = 0.0f;
		DurationEffectRemaining = 0.0f;
	}
		
	if(EndDurationSkillDelegate.IsBound() == true)
	{
		EndDurationSkillDelegate.Broadcast();
	}
}

void UPRBaseSkill::UpdateDurationEffect(float DeltaTime)
{
	if(bActivateDurationEffect)
	{
		DurationEffectElapsed += DeltaTime;
		DurationEffectRemaining = SkillInfo.Duration - DurationEffectElapsed;

		// 지속시간이 지난 후 효과를 종료합니다.
		if(DurationEffectElapsed >= SkillInfo.Duration)
		{
			EndDurationEffect();
		}
	}
}
#pragma endregion 

