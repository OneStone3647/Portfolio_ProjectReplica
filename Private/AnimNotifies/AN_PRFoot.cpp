// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRFoot.h"
#include "AnimInstances/PRBaseAnimInstance.h"

UAN_PRFoot::UAN_PRFoot()
	: Super()
{
	FootDown = EPRFoot::Foot_Left;
}

void UAN_PRFoot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(IsValid(MeshComp) == true)
	{
		UPRBaseAnimInstance* PRAnimInstance = Cast<UPRBaseAnimInstance>(MeshComp->GetAnimInstance());
		if(IsValid(PRAnimInstance) == true)
		{
			PRAnimInstance->SetFootDown(FootDown);
		}
	}
}

FString UAN_PRFoot::GetNotifyName_Implementation() const
{
	FString NewNotifyName;

	NewNotifyName.Append(PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRFoot"), static_cast<uint8>(FootDown)));
	if(NewNotifyName == TEXT("Invalid"))
	{
		return Super::GetNotifyName_Implementation();
	}

	return NewNotifyName;
}
