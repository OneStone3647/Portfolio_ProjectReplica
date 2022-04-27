// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetAerial.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"

UAN_PRSetAerial::UAN_PRSetAerial()
	: Super()
{
	bIsAerial = false;
}

void UAN_PRSetAerial::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		PROwner->GetMovementSystem()->ExecuteAerial(bIsAerial);
	}
}

FString UAN_PRSetAerial::GetNotifyName_Implementation() const
{
	FString NewNotifyName = TEXT("SetAerrial: ");

	if(bIsAerial)
	{
		NewNotifyName.Append(TEXT("True"));
	}
	else
	{
		NewNotifyName.Append(TEXT("False"));
	}
	
	return NewNotifyName;
}
