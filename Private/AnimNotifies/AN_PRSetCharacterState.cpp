// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetCharacterState.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"

UAN_PRSetCharacterState::UAN_PRSetCharacterState()
	: Super()
{
	bIsHit = false;
}

void UAN_PRSetCharacterState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		PROwner->GetStateSystem()->SetIsHit(bIsHit);
	}
}

FString UAN_PRSetCharacterState::GetNotifyName_Implementation() const
{
	FString NewNotifyName = TEXT("SetIsHit: ");

	if(bIsHit)
	{
		NewNotifyName.Append(TEXT("True"));
	}
	else
	{
		NewNotifyName.Append(TEXT("False"));
	}
	
	return NewNotifyName;
}
