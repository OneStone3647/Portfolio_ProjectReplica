// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetCanState.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Libraries/PREnumFunctionLibrary.h"

UAN_PRSetCanState::UAN_PRSetCanState()
	: Super()
{
	CanStates.Empty();
}

void UAN_PRSetCanState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{

		for(FPRCanState CanState : CanStates)
		{
			SetCanState(PROwner, CanState);
		}
	}
}

FString UAN_PRSetCanState::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName;
	int32 Index = 0;

	if(CanStates.Num() == 0)
	{
		return Super::GetNotifyName_Implementation();
	}

	for(FPRCanState CanState : CanStates)
	{
		if(Index != 0)
		{
			NewNotifyName.Append(TEXT(", "));
		}

		NewNotifyName.Append(EnumFunctionLibrary->GetEnumDisplayNameToString(TEXT("EPRCanState"), static_cast<uint8>(CanState.CanState)));
		if(CanState.bIsCanState == true)
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

void UAN_PRSetCanState::SetCanState(APRBaseCharacter* PRCharacter, FPRCanState PRCanState)
{
	switch(PRCanState.CanState)
	{
	case EPRCanState::CanState_CanMove:
		PRCharacter->GetStateSystem()->SetCanMove(PRCanState.bIsCanState);
		break;
	case EPRCanState::CanState_CanDodge:
		PRCharacter->GetStateSystem()->SetCanDodge(PRCanState.bIsCanState);
		break;
	case EPRCanState::CanState_CanSwapWeapon:
		PRCharacter->GetWeaponSystem()->SetCanSwapWeapon(PRCanState.bIsCanState);
		break;
	default:
		break;
	}
}
