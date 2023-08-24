// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PRNiagaraEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

void UPRNiagaraEffect::Activate()
{
	Super::Activate();

	if(NiagaraEffect != nullptr)
	{
		NiagaraEffect->Activate();
	}
}

void UPRNiagaraEffect::Deactivate()
{
	Super::Deactivate();

	if(NiagaraEffect != nullptr)
	{
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
	Super::SpawnEffectAtLocation(Location, Rotation, Scale, bAutoActivate);

	if(NiagaraEffect != nullptr)
	{
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
	Super::SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bAutoActivate);

	if(NiagaraEffect != nullptr && AttachToComponent != nullptr)
	{
		NiagaraEffect->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachPointName);
		if (LocationType == EAttachLocation::KeepWorldPosition)
		{
			NiagaraEffect->SetWorldLocationAndRotation(Location, Rotation);
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
	return Super::GetEffectLocation();
}

UNiagaraComponent* UPRNiagaraEffect::GetNiagaraEffect() const
{
	return NiagaraEffect;
}

void UPRNiagaraEffect::SetNiagaraEffect(UNiagaraComponent* NewNiagaraEffect)
{
	NiagaraEffect = NewNiagaraEffect;
}
