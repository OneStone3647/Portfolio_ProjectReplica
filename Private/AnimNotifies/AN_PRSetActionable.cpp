// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetActionable.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"

UAN_PRSetActionable::UAN_PRSetActionable()
	: Super()
{
	bIsNativeBranchingPoint = true;
	ActionStates.Empty();
}

void UAN_PRSetActionable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	for(const auto& ActionState : ActionStates)
	{
		SetActionable(MeshComp, ActionState.Key, ActionState.Value);
	}
}

FString UAN_PRSetActionable::GetNotifyName_Implementation() const
{
	if(ActionStates.Num() == 0)
	{
		return Super::GetNotifyName_Implementation();
	}

	FString NewNotifyName;
	int32 Index = 0;
	
	for(const auto& ActionState : ActionStates)
	{
		if(Index != 0)
		{
			NewNotifyName.Append(TEXT(", "));
		}

		NewNotifyName.Append(PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRAction"), static_cast<uint8>(ActionState.Key)));
		if(ActionState.Value == true)
		{
			NewNotifyName.Append(TEXT(": True"));
		}
		else
		{
			NewNotifyName.Append(TEXT(": False"));
		}

		Index++;
	}
	
	return NewNotifyName;
}

void UAN_PRSetActionable::SetActionable(USkeletalMeshComponent* MeshComp, EPRAction Action, bool bAble)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(PROwner != nullptr)
		{
			PROwner->GetStateSystem()->SetActionable(Action, bAble);
		}
	}
}
