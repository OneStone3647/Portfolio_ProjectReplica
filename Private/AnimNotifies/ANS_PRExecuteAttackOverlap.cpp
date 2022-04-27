// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRExecuteAttackOverlap.h"
#include "Characters/PRPlayerCharacter.h"
#include "Weapons/PROverlapCollision.h"

UANS_PRExecuteAttackOverlap::UANS_PRExecuteAttackOverlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
	
	// OverlapCollision
	AttackOverlapCollision = nullptr;
	OverlapSize = FVector(1.0f, 1.0f, 1.0f);
}

void UANS_PRExecuteAttackOverlap::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRPlayerCharacter* PRPlayerOwner = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayerOwner) == true)
		{
			AttackOverlapCollision = PRPlayerOwner->GetAttackOverlapCollision();
			if(IsValid(AttackOverlapCollision) == true)
			{
				AttackOverlapCollision->AttachToComponent(PRPlayerOwner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				AttackOverlapCollision->SetActorRelativeLocation(FVector::ZeroVector);
				AttackOverlapCollision->SetActorScale3D(OverlapSize);
				AttackOverlapCollision->ExecuteOverlap(true);
			}
		}
	}
}

void UANS_PRExecuteAttackOverlap::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);

	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if(IsValid(AttackOverlapCollision) == true)
		{
			AttackOverlapCollision->ExecuteOverlap(false);
			AttackOverlapCollision->ClearHitActors();
			AttackOverlapCollision->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
}
