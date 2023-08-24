// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRSkill_Kyle_JudgementCut.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"

UPRSkill_Kyle_JudgementCut::UPRSkill_Kyle_JudgementCut()
{
	// PRAnimMontage
	JudgementCutPRAnimMontage = FPRAnimMontage();
	JudgementCutPRAnimMontageID = 1113;
}

void UPRSkill_Kyle_JudgementCut::InitializeSkill_Implementation()
{
	Super::InitializeSkill_Implementation();

	ActivateableCount = SkillInfo.MaxActivatableCount;

	// 데이터 테이블에서 스킬이 사용하는 PRAnimMontage를 가져옵니다.
	// JudgementCutPRAnimMontage를 데이터 테이블에서 받아옵니다.
	if(IsValid(GetSkillOwner()) == true && JudgementCutPRAnimMontage == FPRAnimMontage())
	{
		JudgementCutPRAnimMontage = FPRAnimMontage(GetSkillOwner()->GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByID(JudgementCutPRAnimMontageID));
	}
}

bool UPRSkill_Kyle_JudgementCut::ActivateSkill_Implementation()
{
	if(IsCanActivateSkill() == true)
	{
		// 일반 공격을 초기화합니다.
		GetSkillOwner()->InitializePlayNormalAttackIndex();
		
		// 캐릭터를 공격하는 방향으로 회전시킵니다.
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(PRPlayerCharacter != nullptr)
		{
			PRPlayerCharacter->RotationAutoTargetDirection();
			// TargetingSystem의 동적 CameraLock을 활성화합니다.
			PRPlayerCharacter->GetTargetingSystem()->SetActivateDynamicCameraLock(true);

			if(PRPlayerCharacter->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
			{
				PRPlayerCharacter->GetMovementSystem()->ActivateAerial(true);
			}
		}
		
		ActivateableCount--;

		// 스킬을 실행하는 동안 다른 동작을 할 수 없도록 설정합니다.
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
		GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_DodgeAttack, false);
		UPRBaseSkill* DodgeAttackSkill = GetSkillOwner()->GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_DodgeAttack);
		if(IsValid(DodgeAttackSkill) == true)
		{
			DodgeAttackSkill->SetActivateSkill(false);
		}

		// 무기를 발도합니다.
		GetSkillOwner()->GetWeaponSystem()->DrawWeapon(0);

		// 재사용 대기시간을 실행합니다.
		if(IsCooldown() == false)
		{
			ActivateCooldown();
		}

		GetSkillOwner()->GetAnimSystem()->PlayPRAnimMontage(JudgementCutPRAnimMontage);
		return true;
	}

	return false;
}

bool UPRSkill_Kyle_JudgementCut::IsCanActivateSkill_Implementation() const
{
	return IsValid(GetSkillOwner()) == true
			&& IsCanActivatableType()
			&& ActivateableCount > 0
			&& GetSkillOwner()->GetStateSystem()->IsActionable(EPRAction::Action_Attack) == true;
}

void UPRSkill_Kyle_JudgementCut::EndCoolDown_Implementation()
{
	Super::EndCoolDown_Implementation();

	if(ActivateableCount + 1 > SkillInfo.MaxActivatableCount == false)
	{
		ActivateableCount++;
		// 스택형 스킬일 경우
		if(ActivateableCount > -1 && ActivateableCount < SkillInfo.MaxActivatableCount)
		{
			ActivateCooldown();
		}
	}
}
