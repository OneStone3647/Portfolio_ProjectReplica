// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRActivateAerial.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"

UAN_PRActivateAerial::UAN_PRActivateAerial()
	: Super()
{
	bIsNativeBranchingPoint = true;
	bActivateAerial = false;
}

void UAN_PRActivateAerial::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ActivateAerial(MeshComp);
}

FString UAN_PRActivateAerial::GetNotifyName_Implementation() const
{
	FString NewNotifyName;

	if(bActivateAerial)
	{
		NewNotifyName.Append(TEXT("Activate Aerial"));
	}
	else
	{
		NewNotifyName.Append(TEXT("Deactivate Aerial"));
	}
	
	return NewNotifyName;
}

void UAN_PRActivateAerial::ActivateAerial(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true)
		{
			PROwner->GetMovementSystem()->ActivateAerial(bActivateAerial);
		}
	}
}
