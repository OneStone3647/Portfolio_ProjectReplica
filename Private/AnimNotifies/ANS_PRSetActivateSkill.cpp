// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRSetActivateSkill.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRSkillSystemComponent.h"
#include "Skills/PRBaseSkill.h"

UANS_PRSetActivateSkill::UANS_PRSetActivateSkill()
	: Super()
{
	CommandSkill = EPRCommandSkill::CommandSkill_None;
	bActivate = false;
}

void UANS_PRSetActivateSkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	SetActivateSkill(MeshComp, CommandSkill, bActivate);
}

void UANS_PRSetActivateSkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	SetActivateSkill(MeshComp, CommandSkill, !bActivate);
}

FString UANS_PRSetActivateSkill::GetNotifyName_Implementation() const
{
	if(CommandSkill == EPRCommandSkill::CommandSkill_None)
	{
		return Super::GetNotifyName_Implementation();
	}

	FString NewNotifyName;
	NewNotifyName.Append(PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRCommandSkill"), static_cast<uint8>(CommandSkill)));
	if(bActivate)
	{
		NewNotifyName.Append(TEXT(": Activate"));
	}
	else
	{
		NewNotifyName.Append(TEXT(": Deactivate"));
	}

	return NewNotifyName;
}

void UANS_PRSetActivateSkill::SetActivateSkill(USkeletalMeshComponent* MeshComp, EPRCommandSkill NewCommandSkill, bool bNewActivate)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(PROwner != nullptr && IsValid(PROwner->GetSkillSystem()->GetSkillFromCommand(NewCommandSkill)) == true)
		{
			PROwner->GetSkillSystem()->GetSkillFromCommand(NewCommandSkill)->SetActivateSkill(bNewActivate);
		}
	}
}
