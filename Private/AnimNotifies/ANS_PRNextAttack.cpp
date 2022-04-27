// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRNextAttack.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Enums/Enum_PRWeaponEquipPosition.h"
#include "Weapons/PRBaseWaepon.h"

UANS_PRNextAttack::UANS_PRNextAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

void UANS_PRNextAttack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true)
		{
			PROwner->GetStateSystem()->SetOnAttack(false);
			PROwner->GetStateSystem()->SetOnDodge(false);
			PROwner->GetWeaponSystem()->SetCanSwapWeapon(true);
		}
	}
}

void UANS_PRNextAttack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);

	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true && PROwner->GetStateSystem()->IsOnAttack() == false)
		{
			PROwner->GetWeaponSystem()->GetEquipWeapon(EPRWeaponEquipPosition::WeaponEquipPosition_Main)->InitializeWeaponPRAnimMontageIndex();
			PROwner->GetStateSystem()->SetCanMove(true);
		}
	}
}
