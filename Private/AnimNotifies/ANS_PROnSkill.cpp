// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PROnSkill.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRSkillSystemComponent.h"
#include "Skills/PRBaseSkill.h"

UANS_PROnSkill::UANS_PROnSkill()
	: Super()
{
	bIsNativeBranchingPoint = true;
	ActivateSkill = nullptr;
}

void UANS_PROnSkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	OnSkillBegin(MeshComp);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_PROnSkill::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	OnSkillTick(MeshComp, FrameDeltaTime);
	
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UANS_PROnSkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnSkillEnd(MeshComp);
	
	Super::NotifyEnd(MeshComp, Animation);
}

void UANS_PROnSkill::OnSkillBegin(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp->GetOwner() != nullptr)
	{
		APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PRCharacter) == true)
		{
			UPRBaseSkill* NewActivateSkill = PRCharacter->GetSkillSystem()->GetSkill(ActivateSkill);
			if(NewActivateSkill != nullptr)
			{
				NewActivateSkill->OnSkillNotifyBegin();
			}
		}
	}
}

void UANS_PROnSkill::OnSkillTick(USkeletalMeshComponent* MeshComp, float DeltaTime)
{
	if(MeshComp->GetOwner() != nullptr)
	{
		APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PRCharacter) == true)
		{
			UPRBaseSkill* NewActivateSkill = PRCharacter->GetSkillSystem()->GetSkill(ActivateSkill);
			if(NewActivateSkill != nullptr)
			{
				NewActivateSkill->OnSkillNotifyTick(DeltaTime);
			}
		}
	}
}

void UANS_PROnSkill::OnSkillEnd(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp->GetOwner() != nullptr)
	{
		APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PRCharacter) == true)
		{
			UPRBaseSkill* NewActivateSkill = PRCharacter->GetSkillSystem()->GetSkill(ActivateSkill);
			if(NewActivateSkill != nullptr)
			{
				NewActivateSkill->OnSkillNotifyEnd();
			}
		}
	}
}

