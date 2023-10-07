// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/PRCooldownSkill.h"
#include "Characters/PRBaseCharacter.h"

UPRCooldownSkill::UPRCooldownSkill()
{
	// TickableGameObject
	bTickable = true;

	// Cooldown
	bActivateCooldown = false;
	CooldownRemaining = 0.0f;
	CooldownElapsed = 0.0f;
}

#pragma region TickableGameObject
void UPRCooldownSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(SkillInfo.bIgnoreTimeStop)
	{
		UpdateCooldown(DeltaTime);
	}
}
#pragma endregion 

bool UPRCooldownSkill::ActivateSkill_Implementation()
{
	return Super::ActivateSkill_Implementation();
}

#pragma region Cooldown
void UPRCooldownSkill::ActivateCooldown_Implementation()
{
	if(IsCooldown() == false)
	{
		if(SkillInfo.bIgnoreTimeStop)
		{
			// TimeStop을 무시할 때
			bActivateCooldown = true;
			CooldownRemaining = SkillInfo.Cooldown;
			CooldownElapsed = 0.0f;
			
		}
		else
		{
			// TimeStop을 적용할 때
			GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				EndCoolDown();
		
			}), SkillInfo.Cooldown, false);
		}
	}
}

bool UPRCooldownSkill::IsCooldown() const
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 떄
		return bActivateCooldown;
	}
	else
	{
		// TimeStop을 적용할 때
		if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
		{
			return true;
		}
	}

	return false;
}

float UPRCooldownSkill::GetSkillCooldownRemaining() const
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 떄
		return CooldownRemaining;
	}
	else
	{
		// TimeStop을 적용할 때
		if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
		{
			return GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
		}

	}
	
	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownRemainingRatio() const
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 떄
		return FMath::Clamp(CooldownRemaining / SkillInfo.Cooldown, 0.0f, 1.0f);
	}
	else
	{
		// TimeStop을 적용할 때
		if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
		{
			return FMath::Clamp(GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle) / SkillInfo.Cooldown, 0.0f, 1.0f);
		}
	}
	
	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownElapsed() const
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 떄
		return CooldownElapsed;
	}
	else
	{
		// TimeStop을 적용할 때
		if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
		{
			return GetWorld()->GetTimerManager().GetTimerElapsed(CooldownTimerHandle);
		}
	}

	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownElapsedRatio() const
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 떄
		return FMath::Clamp(CooldownElapsed / SkillInfo.Cooldown , 0.0f, 1.0f);
	}
	else
	{
		// TimeStop을 적용할 때
		if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
		{
			return FMath::Clamp(GetWorld()->GetTimerManager().GetTimerElapsed(CooldownTimerHandle) / SkillInfo.Cooldown, 0.0f, 1.0f);
		}
	}

	return 0.0f;
}

void UPRCooldownSkill::EndCoolDown_Implementation()
{
	if(SkillInfo.bIgnoreTimeStop)
	{
		// TimeStop을 무시할 때
		CooldownRemaining = SkillInfo.Cooldown;
		CooldownElapsed = 0.0f;
	}
	else
	{
		// TimeStop을 적용할 때
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}
}

void UPRCooldownSkill::UpdateCooldown(float DeltaTime)
{
	if(bActivateCooldown)
	{
		CooldownElapsed += DeltaTime;
		CooldownRemaining = SkillInfo.Cooldown - CooldownElapsed;
		
		// 재사용 대기시간이 끝났을 때
		if(CooldownElapsed >= SkillInfo.Cooldown)
		{
			bActivateCooldown = false;
			EndCoolDown();
		}
	}
}
#pragma endregion 
