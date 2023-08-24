// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRTimedNiagaraEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRBaseCharacter.h"

UANS_PRTimedNiagaraEffect::UANS_PRTimedNiagaraEffect()
	: Super()
{
	Template = nullptr;
	LocationOffset = FVector::ZeroVector;
	RotationOffset = FRotator::ZeroRotator;
	EffectColor = FLinearColor::White;
	EffectSize = 1.0f;
	bDestroyAtEnd = false;
}

void UANS_PRTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// Effect를 스폰하지 않았으면 스폰합니다.
	if(GetSpawnedEffect(MeshComp) == nullptr)
	{
		SpawnEffect(MeshComp, Animation);
	}
	// Effect를 스폰했다면 Effect를 가져와 활성화합니다.
	else
	{
		GetSpawnedEffect(MeshComp)->Activate(true);
	}
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_PRTimedNiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UNiagaraComponent* NiagaraComponent = GetSpawnedEffect(MeshComp);
	if(IsValid(NiagaraComponent) == true)
	{
		if(bDestroyAtEnd)
		{
			NiagaraComponent->DestroyComponent();
		}
		else
		{
			NiagaraComponent->Deactivate();
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

UNiagaraComponent* UANS_PRTimedNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp,	UAnimSequenceBase* Animation) const
{
	if(IsValid(MeshComp) == true && Template != nullptr)
	{
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset,
																								RotationOffset, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
		if(EffectColor == FLinearColor::White)
		{
			APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
			if(IsValid(PROwner) == true)
			{
				SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", PROwner->GetSignatureEffectColor());
			}
			else
			{
				SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", EffectColor);
			}
		}
		else
		{
			SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", EffectColor);
		}
		SpawnNiagaraComponent->SetVariableFloat("EffectSize", EffectSize);
		
		return SpawnNiagaraComponent;
	}

	return nullptr;
}

UNiagaraComponent* UANS_PRTimedNiagaraEffect::GetSpawnedEffect(UMeshComponent* MeshComp)
{
	if(IsValid(MeshComp) == true)
	{
		TArray<USceneComponent*> Children;
		MeshComp->GetChildrenComponents(false, Children);
		if(Children.Num() != 0)
		{
			for(USceneComponent* Component : Children)
			{
				UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(Component);
				if(NiagaraComponent
					&& NiagaraComponent->GetAsset() == Template
					&& NiagaraComponent->GetAttachSocketName() == SocketName)
				{
					return NiagaraComponent;
				}
			}
		}
	}

	return nullptr;
}

