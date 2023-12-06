// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetActiveFootIK.h"
#include "AnimInstances/PRBaseAnimInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"

UAN_PRSetActiveFootIK::UAN_PRSetActiveFootIK()
	: Super()
{
	bActive = false;
}

void UAN_PRSetActiveFootIK::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ActiveFootIK(MeshComp);
}

FString UAN_PRSetActiveFootIK::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	NewNotifyName.Append(TEXT("IsActiveFootIK: "));

	if(bActive)
	{
		NewNotifyName.Append(TEXT("true"));
	}
	else
	{
		NewNotifyName.Append(TEXT("false"));
	}
	
	return NewNotifyName;
}

void UAN_PRSetActiveFootIK::ActiveFootIK(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		UPRBaseAnimInstance* PRBaseAnimInstance = Cast<UPRBaseAnimInstance>(MeshComp->GetAnimInstance());
		if(PRBaseAnimInstance != nullptr)
		{
			PRBaseAnimInstance->ActivateFootIK();
		}
	}
}
