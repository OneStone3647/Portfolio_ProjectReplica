// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetMeshVisibility.h"

UAN_PRSetMeshVisibility::UAN_PRSetMeshVisibility()
	: Super()
{
	bSetMeshVisibility = false;
}

void UAN_PRSetMeshVisibility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(IsValid(MeshComp) == true)
	{
		MeshComp->SetVisibility(bSetMeshVisibility);
	}
}

FString UAN_PRSetMeshVisibility::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	if(bSetMeshVisibility)
	{
		NewNotifyName.Append(TEXT("Visible Mesh"));
	}
	else
	{
		NewNotifyName.Append(TEXT("Hide Mesh"));
	}
		
	return NewNotifyName;
}
