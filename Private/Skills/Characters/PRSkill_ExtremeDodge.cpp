// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/PRSkill_ExtremeDodge.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTimeStopSystemComponent.h"

UPRSkill_ExtremeDodge::UPRSkill_ExtremeDodge()
{
}

bool UPRSkill_ExtremeDodge::ActivateSkill_Implementation()
{
	if(Super::ActivateSkill_Implementation() == true)
	{
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(PRPlayerCharacter != nullptr)
		{
			PRPlayerCharacter->ActivateTimeStop(SkillInfo.Duration);
		}
	}
	
	return false;
}

bool UPRSkill_ExtremeDodge::IsCanActivateSkill_Implementation() const
{
	return Super::IsCanActivateSkill_Implementation()
			&& GetSkillOwner()->GetStateSystem()->IsDead() == false
			&& GetSkillOwner()->GetStateSystem()->IsInvincible() == true;
}
