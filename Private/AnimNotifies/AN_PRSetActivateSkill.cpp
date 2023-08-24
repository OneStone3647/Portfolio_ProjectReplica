// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetActivateSkill.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRSkillSystemComponent.h"

UAN_PRSetActivateSkill::UAN_PRSetActivateSkill()
	: Super()
{
	CommandSkill = EPRCommandSkill::CommandSkill_None;
	bActivate = false;
}

void UAN_PRSetActivateSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	SetActivateSkill(MeshComp, CommandSkill, bActivate);
}

FString UAN_PRSetActivateSkill::GetNotifyName_Implementation() const
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

void UAN_PRSetActivateSkill::SetActivateSkill(USkeletalMeshComponent* MeshComp, EPRCommandSkill NewCommandSkill, bool bNewActivate)
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
