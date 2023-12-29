// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRPlayParticleEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "Effect/PRParticleEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

UParticleSystemComponent* UAN_PRPlayParticleEffect::SpawnParticleSystem(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UParticleSystemComponent* ReturnComp = nullptr;

	if(PSTemplate)
	{
		if(PSTemplate->IsLooping())
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
				if(EffectSystem->IsCreateParticleEffectPool(PSTemplate) == true)
				{
					UPRParticleEffect* SpawnPRParticleEffect = nullptr;

					if(Attached)
					{
						SpawnPRParticleEffect = EffectSystem->SpawnParticleEffectAttached(PSTemplate, MeshComp, SocketName, LocationOffset, RotationOffset, Scale, EAttachLocation::KeepWorldPosition, true);
					}
					else
					{
						const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
						SpawnPRParticleEffect = EffectSystem->SpawnParticleEffectAtLocation(PSTemplate, MeshTransform.TransformPosition(LocationOffset), (MeshTransform.GetRotation() * FQuat(RotationOffset)).Rotator(), FVector(1.0f),true);
					}

					return ReturnComp;
				}
			}
		}

		// EffectSystem에 Effect가 존재하지 않을 경우 일반적인 방법으로 Effect를 Spawn합니다.
		if (Attached)
		{
			ReturnComp = UGameplayStatics::SpawnEmitterAttached(PSTemplate, MeshComp, SocketName, LocationOffset, RotationOffset, Scale);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(MeshTransform.TransformPosition(LocationOffset));
			SpawnTransform.SetRotation(MeshTransform.GetRotation() * FQuat(RotationOffset));
			SpawnTransform.SetScale3D(Scale);
			ReturnComp = UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld(), PSTemplate, SpawnTransform);
		}
	}

	return ReturnComp;
}
