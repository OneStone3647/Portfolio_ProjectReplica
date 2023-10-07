// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRActivateExtremeDodge.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/SphereComponent.h"

UANS_PRActivateExtremeDodge::UANS_PRActivateExtremeDodge()
	: Super()
{
	bIsNativeBranchingPoint = true;

	OwnerPRPlayerCharacter = nullptr;
	bActivateDebug = false;
	DebugDuration = 5.0f;
}

void UANS_PRActivateExtremeDodge::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	APRPlayerCharacter* NewOwnerPRPlayerCharacter = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
	if(IsValid(NewOwnerPRPlayerCharacter) == true)
	{
		OwnerPRPlayerCharacter = NewOwnerPRPlayerCharacter;
		NewOwnerPRPlayerCharacter->ActivateExtremeDodgeArea();

		// Debug 실행
		if(bActivateDebug)
		{
			ActivateDebug();
		}
	}
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_PRActivateExtremeDodge::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if(OwnerPRPlayerCharacter != nullptr)
	{
		// Debug 실행
		if(bActivateDebug)
		{
			ActivateDebug();
		}
	}
	
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UANS_PRActivateExtremeDodge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// 플레이어 캐릭터의 무적 상태를 해제합니다.
	if(OwnerPRPlayerCharacter != nullptr && OwnerPRPlayerCharacter->GetStateSystem()->IsInvincible() == true)
	{
		OwnerPRPlayerCharacter->GetStateSystem()->SetIsInvincible(false);
		OwnerPRPlayerCharacter->DeactivateExtremeDodgeArea();

		// Debug 실행
		if(bActivateDebug)
		{
			ActivateDebug();
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation);
}

FString UANS_PRActivateExtremeDodge::GetNotifyName_Implementation() const
{
	return TEXT("Activate Extreme Dodge");
}

void UANS_PRActivateExtremeDodge::ActivateDebug()
{
	if(OwnerPRPlayerCharacter != nullptr)
	{
		UKismetSystemLibrary::DrawDebugSphere(OwnerPRPlayerCharacter->GetWorld(), OwnerPRPlayerCharacter->GetActorLocation(),
												OwnerPRPlayerCharacter->GetExtremeDodgeArea()->GetScaledSphereRadius(), 12 ,FLinearColor::Green,
												DebugDuration);
	}
}
