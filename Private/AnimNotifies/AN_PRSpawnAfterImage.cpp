// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSpawnAfterImage.h"
#include "Characters/PRPlayerCharacter.h"
#include "Effect/PRAfterImage.h"

void UAN_PRSpawnAfterImage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	SpawnAfterImage(MeshComp);
}

void UAN_PRSpawnAfterImage::SpawnAfterImage(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true)
	{
		APRPlayerCharacter* OwnerPRPlayerCharacter = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(OwnerPRPlayerCharacter != nullptr)
		{
			OwnerPRPlayerCharacter->ActivateAfterImage();
		}
	}
}
