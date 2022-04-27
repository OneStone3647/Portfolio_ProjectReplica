// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetOnState.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Libraries/PREnumFunctionLibrary.h"

UAN_PRSetOnState::UAN_PRSetOnState()
	: Super()
{
	OnStates.Empty();
}

void UAN_PRSetOnState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		for(FPROnState OnState : OnStates)
		{
			SetOnState(PROwner, OnState);
		}
	}
}

FString UAN_PRSetOnState::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName;
	int32 Index = 0;

	if(OnStates.Num() == 0)
	{
		return Super::GetNotifyName_Implementation();
	}
	
	for(FPROnState OnState : OnStates)
	{
		if(Index != 0)
		{
			NewNotifyName.Append(TEXT(", "));
		}

		NewNotifyName.Append(EnumFunctionLibrary->GetEnumDisplayNameToString(TEXT("EPROnState"), static_cast<uint8>(OnState.OnState)));
		if(OnState.bIsOnState == true)
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

void UAN_PRSetOnState::SetOnState(APRBaseCharacter* PRCharacter, FPROnState PROnState)
{
	switch(PROnState.OnState)
	{
	case EPROnState::OnState_OnDodge:
		PRCharacter->GetStateSystem()->SetOnDodge(PROnState.bIsOnState);
		break;
	case EPROnState::OnState_OnAttack:
		PRCharacter->GetStateSystem()->SetOnAttack(PROnState.bIsOnState);
		break;
	default:
		break;
	}
}
