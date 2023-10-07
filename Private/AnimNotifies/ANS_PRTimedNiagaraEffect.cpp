// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRTimedNiagaraEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effect/PRNiagaraEffect.h"

UANS_PRTimedNiagaraEffect::UANS_PRTimedNiagaraEffect()
	: Super()
{
	Template = nullptr;
	SpawnedPRNiagaraEffect = nullptr;
	SpawnedNiagaraEffect = nullptr;
	SocketName = FName();
	LocationOffset = FVector::ZeroVector;
	RotationOffset = FRotator::ZeroRotator;
	Scale = FVector(1.0f);
	bDestroyAtEnd = false;
}

void UANS_PRTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	SpawnEffect(MeshComp, Animation);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_PRTimedNiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(SpawnedPRNiagaraEffect != nullptr)
	{
		SpawnedPRNiagaraEffect->Deactivate();
	}
	else if(SpawnedNiagaraEffect != nullptr)
	{
		if(bDestroyAtEnd)
		{
			SpawnedNiagaraEffect->DestroyComponent();
		}
		else
		{
			SpawnedNiagaraEffect->Deactivate();
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation);
}

FString UANS_PRTimedNiagaraEffect::GetNotifyName_Implementation() const
{
	if(Template != nullptr)
	{
		return Template->GetName();
	}
	
	return Super::GetNotifyName_Implementation();
}

void UANS_PRTimedNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(IsValid(MeshComp) == true && Template != nullptr)
	{
		// PRBaseCharacter일 경우
		APRBaseCharacter* PRBaseCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(PRBaseCharacter != nullptr)
		{
			SpawnedPRNiagaraEffect = PRBaseCharacter->GetEffectSystem()->SpawnNiagaraEffectAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset,
																									Scale, EAttachLocation::KeepRelativeOffset);
			if(SpawnedPRNiagaraEffect != nullptr)
			{
				return;
			}
		}

		// PRBaseCharacter가 아니거나 EffectSystem에 Spawn하고자 하는 Effect가 없을 경우
		SpawnedNiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset,
																				EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
	}
}

