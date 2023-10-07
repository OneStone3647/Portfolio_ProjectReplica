// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetIsInvincible.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"

UAN_PRSetIsInvincible::UAN_PRSetIsInvincible()
	: Super()
{
	bIsNativeBranchingPoint = true;

	bIsInvincible = false;
}

void UAN_PRSetIsInvincible::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APRBaseCharacter* PRBaseCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PRBaseCharacter) == true)
	{
		PRBaseCharacter->GetStateSystem()->SetIsInvincible(bIsInvincible);
	}
	
	Super::Notify(MeshComp, Animation);
}

FString UAN_PRSetIsInvincible::GetNotifyName_Implementation() const
{
	FString NewNotifyName = Super::GetNotifyName_Implementation();
	if(bIsInvincible)
	{
		NewNotifyName.Append(TEXT(": True"));
	}
	else
	{
		NewNotifyName.Append(TEXT(": False"));
	}
	
	return NewNotifyName;
}
