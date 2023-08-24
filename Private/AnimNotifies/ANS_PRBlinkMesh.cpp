// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRBlinkMesh.h"

UANS_PRBlinkMesh::UANS_PRBlinkMesh()
	: Super()
{
	bBlinkReverse = false;
}

void UANS_PRBlinkMesh::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if(IsValid(MeshComp) == true)
	{
		MeshComp->SetVisibility(bBlinkReverse);
	}
}

void UANS_PRBlinkMesh::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if(IsValid(MeshComp) == true)
	{
		MeshComp->SetVisibility(!bBlinkReverse);
	}
}
