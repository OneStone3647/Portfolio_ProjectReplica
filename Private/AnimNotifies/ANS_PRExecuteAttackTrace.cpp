// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRExecuteAttackTrace.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Weapons/PRBaseWaepon.h"
#include "Libraries/PREnumFunctionLibrary.h"

UANS_PRExecuteAttackTrace::UANS_PRExecuteAttackTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;

	WeaponEquipPosition = EPRWeaponEquipPosition::WeaponEquipPosition_Main;
}

void UANS_PRExecuteAttackTrace::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true && PROwner->GetWeaponSystem()->IsEquipWeapon(WeaponEquipPosition) == true)
		{
			PROwner->GetWeaponSystem()->GetEquipWeapon(WeaponEquipPosition)->UpdateAllTraceCollisionsLocation();
		}
	}
}

void UANS_PRExecuteAttackTrace::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	Super::BranchingPointNotifyTick(BranchingPointPayload, FrameDeltaTime);
	
	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true && PROwner->GetWeaponSystem()->IsEquipWeapon(WeaponEquipPosition) == true)
		{
			PROwner->GetWeaponSystem()->GetEquipWeapon(WeaponEquipPosition)->ExecuteAttackTrace();
		}
		
	}
}

void UANS_PRExecuteAttackTrace::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	
	if(USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true && PROwner->GetWeaponSystem()->IsEquipWeapon(WeaponEquipPosition) == true)
		{
			PROwner->GetWeaponSystem()->GetEquipWeapon(WeaponEquipPosition)->ClearHitActors();
		}
	}
}

FString UANS_PRExecuteAttackTrace::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName = TEXT("ExecuteAttackTrace Position: ");

	NewNotifyName.Append(EnumFunctionLibrary->GetEnumDisplayNameToString(TEXT("EPRWeaponEquipPosition"), static_cast<uint8>(WeaponEquipPosition)));

	return NewNotifyName; 
}
