// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PRNiagaraEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UPRNiagaraEffect::UPRNiagaraEffect()
{
	NiagaraEffect = nullptr;
}

void UPRNiagaraEffect::Initialize()
{
	Super::Initialize();

	OnEffectDeactivate.Clear();
	Deactivate();
}

void UPRNiagaraEffect::UpdateEffect(float DeltaTime)
{
	Super::UpdateEffect(DeltaTime);

	if(DeltaTime != 0.0f && NiagaraEffect != nullptr && IsValid(GetEffectOwner()) == true)
	{
		NiagaraEffect->AdvanceSimulation(1, DeltaTime);
	}
}

bool UPRNiagaraEffect::IsActivate() const
{
	if(NiagaraEffect != nullptr)
	{
		return Super::IsActivate() && NiagaraEffect->IsActive();
	}

	return false;
}

void UPRNiagaraEffect::Activate()
{
	Super::Activate();

	if(NiagaraEffect != nullptr)
	{
		NiagaraEffect->SetHiddenInGame(false);
		NiagaraEffect->Activate();
	}
}

void UPRNiagaraEffect::Deactivate()
{
	Super::Deactivate();

	if(NiagaraEffect != nullptr)
	{
		NiagaraEffect->SetHiddenInGame(true);
		NiagaraEffect->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		NiagaraEffect->Deactivate();
	}
}

void UPRNiagaraEffect::Destroy()
{
	Deactivate();
	NiagaraEffect = nullptr;
	
	Super::Destroy();
}

void UPRNiagaraEffect::SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate)
{
	if(NiagaraEffect != nullptr)
	{
		Activate();
		
		NiagaraEffect->SetWorldLocationAndRotation(Location, Rotation);
		NiagaraEffect->SetRelativeScale3D(Scale);
		
		if(bAutoActivate)
		{
			NiagaraEffect->Activate(true);
		}
	}
}

void UPRNiagaraEffect::SpawnEffectAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location,	FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate)
{
	if(NiagaraEffect != nullptr && AttachToComponent != nullptr)
	{
		Activate();
		
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
		NiagaraEffect->AttachToComponent(AttachToComponent, AttachmentTransformRules, AttachPointName);
		if (LocationType == EAttachLocation::KeepWorldPosition)
		{
			NiagaraEffect->SetWorldLocationAndRotation(AttachToComponent->GetSocketLocation(AttachPointName) + Location, AttachToComponent->GetSocketRotation(AttachPointName) + Rotation);
		}
		else
		{
			NiagaraEffect->SetRelativeLocationAndRotation(Location, Rotation);
		}
		NiagaraEffect->SetRelativeScale3D(Scale);

		if(bAutoActivate)
		{
			NiagaraEffect->Activate(true);
		}
	}
}

FVector UPRNiagaraEffect::GetEffectLocation() const
{
	if(NiagaraEffect != nullptr)
	{
		return NiagaraEffect->GetComponentLocation();
	}
	
	return Super::GetEffectLocation();
}

UFXSystemComponent* UPRNiagaraEffect::GetFXSystemComponent() const
{
	return NiagaraEffect;
}

bool UPRNiagaraEffect::IsLooping() const
{
	if(NiagaraEffect != nullptr)
	{
		return NiagaraEffect->GetAsset()->IsLooping();
	}

	return false;
}

UNiagaraComponent* UPRNiagaraEffect::GetNiagaraEffect() const
{
	return NiagaraEffect;
}

void UPRNiagaraEffect::SetNiagaraEffect(UNiagaraComponent* NewNiagaraEffect)
{
	NiagaraEffect = NewNiagaraEffect;
}
