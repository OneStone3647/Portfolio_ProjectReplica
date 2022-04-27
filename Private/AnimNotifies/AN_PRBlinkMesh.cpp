// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRBlinkMesh.h"

UAN_PRBlinkMesh::UAN_PRBlinkMesh()
	: Super()
{
	bBlinkMesh = false;
}

void UAN_PRBlinkMesh::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	MeshComp->SetVisibility(bBlinkMesh);
}

FString UAN_PRBlinkMesh::GetNotifyName_Implementation() const
{
	FString NewNotifyName = TEXT("BlinkMesh: ");

	if(bBlinkMesh)
	{
		NewNotifyName.Append(TEXT("True"));
	}
	else
	{
		NewNotifyName.Append(TEXT("False"));
	}
	
	return NewNotifyName;
}
