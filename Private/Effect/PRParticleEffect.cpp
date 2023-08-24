// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PRParticleEffect.h"
#include "Particles/ParticleSystemComponent.h"

UPRParticleEffect::UPRParticleEffect()
{
	ParticleEffect = nullptr;
}

void UPRParticleEffect::Initialize()
{
	Super::Initialize();

	Deactivate();
}

void UPRParticleEffect::Activate()
{
	Super::Activate();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->Activate();
	}
}

void UPRParticleEffect::Deactivate()
{
	Super::Deactivate();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->Deactivate();
	}
}

void UPRParticleEffect::Destroy()
{
	Deactivate();
	ParticleEffect = nullptr;
	
	Super::Destroy();
}

void UPRParticleEffect::SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate)
{
	Super::SpawnEffectAtLocation(Location, Rotation, Scale, bAutoActivate);

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->SetWorldLocationAndRotation(Location, Rotation);
		ParticleEffect->SetRelativeScale3D(Scale);

		if(bAutoActivate)
		{
			ParticleEffect->Activate(true);
		}
	}
}

void UPRParticleEffect::SpawnEffectAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate)
{
	Super::SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bAutoActivate);

	if(ParticleEffect != nullptr && AttachToComponent != nullptr)
	{
		ParticleEffect->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachPointName);
		if (LocationType == EAttachLocation::KeepWorldPosition)
		{
			ParticleEffect->SetWorldLocationAndRotation(Location, Rotation);
		}
		else
		{
			ParticleEffect->SetRelativeLocationAndRotation(Location, Rotation);
		}
		ParticleEffect->SetRelativeScale3D(Scale);

		if(bAutoActivate)
		{
			ParticleEffect->Activate(true);
		}
	}
}

FVector UPRParticleEffect::GetEffectLocation() const
{
	return Super::GetEffectLocation();
}

UParticleSystemComponent* UPRParticleEffect::GetParticleEffect() const
{
	return ParticleEffect;
}

void UPRParticleEffect::SetParticleEffect(UParticleSystemComponent* NewParticleEffect)
{
	ParticleEffect = NewParticleEffect;
}
