// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRTimedParticleEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effect/PRParticleEffect.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

UANS_PRTimedParticleEffect::UANS_PRTimedParticleEffect()
	: Super()
{
	Template = nullptr;
	SpawnedPRParticleEffect = nullptr;
	SpawnedParticleEffect = nullptr;
	SocketName = FName();
	LocationOffset = FVector::ZeroVector;
	RotationOffset = FRotator::ZeroRotator;
	Scale = FVector(1.0f);
	bDestroyAtEnd = false;
}

void UANS_PRTimedParticleEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	SpawnEffect(MeshComp, Animation);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_PRTimedParticleEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(SpawnedPRParticleEffect != nullptr)
	{
		SpawnedPRParticleEffect->Deactivate();
	}
	else if(SpawnedParticleEffect != nullptr)
	{
		if(bDestroyAtEnd)
		{
			SpawnedParticleEffect->DestroyComponent();
		}
		else
		{
			SpawnedParticleEffect->Deactivate();
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation);
}

FString UANS_PRTimedParticleEffect::GetNotifyName_Implementation() const
{
	if(Template != nullptr)
	{
		return Template->GetName();
	}
	
	return Super::GetNotifyName_Implementation();
}

void UANS_PRTimedParticleEffect::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(IsValid(MeshComp) == true && Template != nullptr)
	{
		// PRBaseCharacter일 경우
		APRBaseCharacter* PRBaseCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(PRBaseCharacter != nullptr)
		{
			SpawnedPRParticleEffect = PRBaseCharacter->GetEffectSystem()->SpawnParticleEffectAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset,
																										Scale, EAttachLocation::KeepRelativeOffset);
			if(SpawnedPRParticleEffect != nullptr)
			{
				return;
			}
		}

		// PRBaseCharacter가 아니거나 EffectSystem에 Spawn하고자 하는 Effect가 없을 경우
		SpawnedParticleEffect = UGameplayStatics::SpawnEmitterAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset,
																		EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
	}
}
