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
	// ExtremeDodgeArea가 활성화되어 ExtremeDodge를 실행할 수 있는지 판별합니다.
	bool bIsCanExtremeDodge = false;
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(PRPlayerCharacter != nullptr)
	{
		bIsCanExtremeDodge = PRPlayerCharacter->IsActivateExtremeDodgeArea();
	}
	
	return Super::IsCanActivateSkill_Implementation()
			&& bIsCanExtremeDodge
			&& GetSkillOwner()->GetStateSystem()->IsDead() == false
			&& GetSkillOwner()->GetStateSystem()->IsInvincible() == true;
}
