// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRNextAttack.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter_Kyle.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"

UANS_PRNextAttack::UANS_PRNextAttack()
	: Super()
{
	bIsNativeBranchingPoint = true;
}

void UANS_PRNextAttack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	
	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if(MeshComp != nullptr)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true)
		{
			PROwner->GetStateSystem()->SetActionable(EPRAction::Action_Attack, true);
		}
	}
}

void UANS_PRNextAttack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	
	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if(MeshComp != nullptr)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true && PROwner->GetStateSystem()->IsActionable(EPRAction::Action_Attack) == true)
		{
			// PROwner->GetWeaponSystem()->SetEquippedWeaponGroupAttackPRAnimMontageIndex(0);
			PROwner->GetStateSystem()->SetActionable(EPRAction::Action_Move, true);

			APRPlayerCharacter* PRPlayerOwner = Cast<APRPlayerCharacter>(PROwner);
			if(IsValid(PRPlayerOwner) == true)
			{
				PRPlayerOwner->InitializePlayNormalAttackIndex();
			}
		}
	}
}
