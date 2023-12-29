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

	OnParticleEffectDeactivate.Clear();
	Deactivate();
}

// void UPRParticleEffect::UpdateEffect(float DeltaTime)
// {
// 	Super::UpdateEffect(DeltaTime);
//
// 	if(ParticleEffect != nullptr && IsValid(GetEffectOwner()) == true)
// 	{
// 		ParticleEffect->CustomTimeDilation = GetEffectOwner()->CustomTimeDilation;
// 	}
// }

bool UPRParticleEffect::IsActivate() const
{
	if(ParticleEffect != nullptr)
	{
		return Super::IsActivate() && ParticleEffect->IsActive();
	}

	return false;
}

void UPRParticleEffect::Activate()
{
	Super::Activate();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->SetHiddenInGame(false);
		ParticleEffect->Activate();
	}
}

void UPRParticleEffect::Deactivate()
{
	Super::Deactivate();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->SetHiddenInGame(true);
		ParticleEffect->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		ParticleEffect->Deactivate();
	}

	if(OnParticleEffectDeactivate.IsBound())
	{
		OnParticleEffectDeactivate.Broadcast(this);
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
	if(ParticleEffect != nullptr)
	{
		Activate();
		
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
	if(ParticleEffect != nullptr && AttachToComponent != nullptr)
	{
		Activate();
		
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
		ParticleEffect->AttachToComponent(AttachToComponent, AttachmentTransformRules, AttachPointName);
		if (LocationType == EAttachLocation::KeepWorldPosition)
		{
			ParticleEffect->SetWorldLocationAndRotation(AttachToComponent->GetSocketLocation(AttachPointName) + Location, AttachToComponent->GetSocketRotation(AttachPointName) + Rotation);
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

UFXSystemComponent* UPRParticleEffect::GetFXSystemComponent() const
{
	return ParticleEffect;
}

bool UPRParticleEffect::IsLooping() const
{
	if(ParticleEffect != nullptr)
	{
		return ParticleEffect->Template->IsLooping();
	}

	return false;
}

void UPRParticleEffect::TimeStopActive()
{
	Super::TimeStopActive();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->CustomTimeDilation = 0.0f;
	}
}

void UPRParticleEffect::TimeStopDeactive()
{
	Super::TimeStopDeactive();

	if(ParticleEffect != nullptr)
	{
		ParticleEffect->CustomTimeDilation = 1.0f;
	}
}

UParticleSystem* UPRParticleEffect::GetParticleEffectAsset() const
{
	if(ParticleEffect != nullptr)
	{
		return Cast<UParticleSystem>(ParticleEffect->GetFXSystemAsset());
	}

	return nullptr;
}

void UPRParticleEffect::SetParticleEffectCustomTimeDilation(float NewCustomTimeDilation)
{
	if(ParticleEffect != nullptr)
	{
		ParticleEffect->CustomTimeDilation = NewCustomTimeDilation;
	}
}

UParticleSystemComponent* UPRParticleEffect::GetParticleEffect() const
{
	return ParticleEffect;
}

void UPRParticleEffect::SetParticleEffect(UParticleSystemComponent* NewParticleEffect)
{
	ParticleEffect = NewParticleEffect;
}
