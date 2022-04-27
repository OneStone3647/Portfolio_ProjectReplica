// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetCollisionPreset.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Libraries/PREnumFunctionLibrary.h"

UAN_PRSetCollisionPreset::UAN_PRSetCollisionPreset()
	: Super()
{
	CollisionPreset = EPRCollisionPreset::CollisionPreset_Pawn;
}

void UAN_PRSetCollisionPreset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		switch(CollisionPreset)
		{
		case EPRCollisionPreset::CollisionPreset_PlayerCharacter:
			PROwner->GetCapsuleComponent()->SetCollisionProfileName("PlayerCharacter");
			break;
		case EPRCollisionPreset::CollisionPreset_GhostCharacter:
			PROwner->GetCapsuleComponent()->SetCollisionProfileName("GhostCharacter");
			break;
		case EPRCollisionPreset::CollisionPreset_Pawn:
		default:
			PROwner->GetCapsuleComponent()->SetCollisionProfileName("Pawn");
			break;
		}
	}
}

FString UAN_PRSetCollisionPreset::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName = TEXT("CollisionPreset: ");

	switch(CollisionPreset)
	{
	case EPRCollisionPreset::CollisionPreset_PlayerCharacter:
		NewNotifyName.Append(TEXT("PlayerCharacter"));
		break;
	case EPRCollisionPreset::CollisionPreset_GhostCharacter:
		NewNotifyName.Append(TEXT("GhostCharacter"));
		break;
	case EPRCollisionPreset::CollisionPreset_Pawn:
	default:
		NewNotifyName.Append(TEXT("Pawn"));
		break;
	}

	return NewNotifyName;
}
