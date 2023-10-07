// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRPlayNiagaraEffect.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effect/PRNiagaraEffect.h"

UFXSystemComponent* UAN_PRPlayNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UFXSystemComponent* ReturnComp = nullptr;

	if(Template)
	{
		if(Template->IsLooping())
		{
			return ReturnComp;
		}

		// EffectSystem에 Effect가 존재할 경우 EffectSystem에서 가져와 Spawn합니다.
		APRBaseCharacter* PRBaseCharacter = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PRBaseCharacter) == true)
		{
			UPREffectSystemComponent* EffectSystem = PRBaseCharacter->GetEffectSystem();
			if(EffectSystem != nullptr)
			{
				// EffectPool에 Spawn할 Effect가 존재하는지 탐색합니다.
				if(EffectSystem->IsValidEffectPool(Template) == true)
				{
					UPRNiagaraEffect* SpawnPRNiagaraEffect = nullptr;

					if(Attached)
					{
						SpawnPRNiagaraEffect = EffectSystem->SpawnNiagaraEffectAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, Scale, EAttachLocation::KeepWorldPosition, true);
					}
					else
					{
						const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
						SpawnPRNiagaraEffect = EffectSystem->SpawnNiagaraEffectAtLocation(Template, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * RotationOffsetQuat).Rotator(), FVector(1.0f),true);
					}

					if(SpawnPRNiagaraEffect != nullptr)
					{
						ReturnComp = SpawnPRNiagaraEffect->GetFXSystemComponent();
						if(ReturnComp != nullptr)
						{
							ReturnComp->SetUsingAbsoluteScale(bAbsoluteScale);
							ReturnComp->SetRelativeScale3D_Direct(Scale);
						}
						
						return ReturnComp;
					}
				}
			}
		}

		// EffectSystem에 Effect가 존재하지 않을 경우 일반적인 방법으로 Effect를 Spawn합니다.
		if(Attached)
		{
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), Template, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * RotationOffsetQuat).Rotator(), FVector(1.0f),true);
		}

		if(ReturnComp != nullptr)
		{
			ReturnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			ReturnComp->SetRelativeScale3D_Direct(Scale);
		}
	}

	return ReturnComp;
}
