// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRSkill_Kyle_DodgeAttack.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"

UPRSkill_Kyle_DodgeAttack::UPRSkill_Kyle_DodgeAttack()
{
	// DodgeAttack
	SkillDurationTime = 3.5f;
	SkillGauge = 70.0f;
	SkillGaugeCost = 10.0f;
	MinSkillGauge = 0.0f;
	MaxSkillGauge = 70.0f;
	
	// PRAnimMontage
	PRComboAnimMontage = FPRComboAnimMontage();
	PRComboAnimMontageFromID = 1106;
	PRComboAnimMontageToID = 1112;
}

void UPRSkill_Kyle_DodgeAttack::InitializeSkill_Implementation()
{
	Super::InitializeSkill_Implementation();

	// 데이터 테이블에서 스킬이 사용하는 PRAnimMontage들을 가져옵니다.
	if(IsValid(GetSkillOwner()) == true)
	{
		// ComboAnimMontage를 데이터 테이블에서 받아옵니다.
		if(PRComboAnimMontage == FPRComboAnimMontage())
		{
			PRComboAnimMontage = FPRComboAnimMontage(GetSkillOwner()->GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToArray(PRComboAnimMontageFromID, PRComboAnimMontageToID));
		}

		PRComboAnimMontage.InitializePlayIndex();
		SkillGauge = MaxSkillGauge;
	}
}

bool UPRSkill_Kyle_DodgeAttack::ActivateSkill_Implementation()
{
	if(IsCanActivateSkill() == true)
	{
		if(IsActivateSkill() == false)
		{
			SetActivateSkill(true);
		}

		// 캐릭터를 공격하는 방향으로 회전시킵니다.
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(PRPlayerCharacter != nullptr)
		{
			PRPlayerCharacter->RotationAutoTargetDirection();
			// TargetingSystem의 동적 CameraLock을 활성화합니다.
			PRPlayerCharacter->GetTargetingSystem()->SetActivateDynamicCameraLock(true);
		}

		GetSkillOwner()->InitializePlayNormalAttackIndex();
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
		GetSkillOwner()->GetAnimSystem()->PlayPRAnimMontage(PRComboAnimMontage.GetPlayPRAnimMontage());
		
		if(SkillGauge - SkillGaugeCost >= 0.0f)
		{
			SkillGauge -= SkillGaugeCost;
		}
		else
		{
			SkillGauge = 0.0f;
		}
		
		// 마지막 ComboAnimMontage일 경우 스킬을 비활성화합니다.
		if(PRComboAnimMontage.GetPlayIndex() + 1 == PRComboAnimMontage.PRAnimMontages.Num())
		{
			SetActivateSkill(false);

			return false;
		}
	
		PRComboAnimMontage.IncreasePlayIndex();

		return true;
	}

	return false;
}

bool UPRSkill_Kyle_DodgeAttack::IsCanActivateSkill_Implementation() const
{
	return Super::IsCanActivateSkill_Implementation()
			&& SkillGauge >= 10.0f
			&& GetSkillOwner()->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == false
			&& GetSkillOwner()->GetStateSystem()->IsActionable(EPRAction::Action_Attack) == true;
}

void UPRSkill_Kyle_DodgeAttack::IncreaseSkillGauge(float IncreaseCost)
{
	if(SkillGauge + IncreaseCost > MaxSkillGauge)
	{
		SkillGauge = MaxSkillGauge;
	}
	else
	{
		SkillGauge += IncreaseCost;
	}
}

void UPRSkill_Kyle_DodgeAttack::SetActivateSkill(bool bNewActivateSkill)
{
	Super::SetActivateSkill(bNewActivateSkill);

	if(bNewActivateSkill)
	{
		ActivateDuration();
	}
	else
	{
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_DodgeAttack, false);
	}
	
	InitializeSkill();
}

void UPRSkill_Kyle_DodgeAttack::EndDurationEffect()
{
	Super::EndDurationEffect();

	SetActivateSkill(false);
}
