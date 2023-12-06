// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PROnSkill.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRSkillSystemComponent.h"
#include "Skills/PRBaseSkill.h"

UAN_PROnSkill::UAN_PROnSkill()
	: Super()
{
	bIsNativeBranchingPoint = true;
	ActivateSkill = nullptr;
}

void UAN_PROnSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnSkill(MeshComp);
	
	Super::Notify(MeshComp, Animation);
}

void UAN_PROnSkill::OnSkill(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp->GetOwner() != nullptr)
 	{
 		APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
 		if(IsValid(PRCharacter) == true)
 		{
 			UPRBaseSkill* NewActivateSkill = PRCharacter->GetSkillSystem()->GetSkill(ActivateSkill);
 			if(NewActivateSkill != nullptr)
 			{
 				NewActivateSkill->OnSkillNotify();
 			}
 		}
 	}
}
