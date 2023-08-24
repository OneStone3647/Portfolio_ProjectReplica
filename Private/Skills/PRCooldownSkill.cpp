// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/PRCooldownSkill.h"
#include "Characters/PRBaseCharacter.h"

bool UPRCooldownSkill::ActivateSkill_Implementation()
{
	return Super::ActivateSkill_Implementation();
}

void UPRCooldownSkill::ActivateCooldown_Implementation()
{
	if(IsCooldown() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			EndCoolDown();
		
		}), SkillInfo.Cooldown, false);
	}
}

bool UPRCooldownSkill::IsCooldown() const
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
	{
		return true;
	}

	return false;
}

float UPRCooldownSkill::GetSkillCooldownRemaining() const
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
	}

	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownRemainingRatio() const
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
	{
		return FMath::Clamp((GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle) / SkillInfo.Cooldown), 0.0f, 1.0f);
	}
	
	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownElapsed() const
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
	{
		return GetWorld()->GetTimerManager().GetTimerElapsed(CooldownTimerHandle);
	}

	return 0.0f;
}

float UPRCooldownSkill::GetSkillCooldownElapsedRatio() const
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandle) == true)
	{
		return FMath::Clamp((GetWorld()->GetTimerManager().GetTimerElapsed(CooldownTimerHandle) / SkillInfo.Cooldown), 0.0f, 1.0f);
	}

	return 0.0f;
}

void UPRCooldownSkill::EndCoolDown_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}
