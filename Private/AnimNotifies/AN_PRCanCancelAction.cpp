// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRCanCancelAction.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"

UAN_PRCanCancelAction::UAN_PRCanCancelAction()
	: Super()
{
	bIsNativeBranchingPoint = true;
	bCanCancelAction = false;
}

void UAN_PRCanCancelAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	SetCanCancelAction(MeshComp);
}

FString UAN_PRCanCancelAction::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	NewNotifyName.Append(TEXT("Can Cancel Action"));
	if(bCanCancelAction == true)
	{
		NewNotifyName.Append(TEXT(": True"));
	}
	else
	{
		NewNotifyName.Append(TEXT(": False"));
	}
	
	return NewNotifyName;
}

void UAN_PRCanCancelAction::SetCanCancelAction(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(PROwner != nullptr)
		{
			PROwner->GetStateSystem()->SetCanCancelAction(bCanCancelAction);
		}
	}
}
