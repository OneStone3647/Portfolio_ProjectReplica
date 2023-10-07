// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PREffect.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"

UPREffect::UPREffect()
{
	bActivate = false;
	EffectSourceAsset = nullptr;
	EffectOwner = nullptr;
	PoolIndex = -1;
	Lifespan = 0.0f;
}

void UPREffect::Initialize()
{
	bActivate = false;
	EffectSourceAsset = nullptr;
	EffectOwner = nullptr;
	PoolIndex = -1;
	Lifespan = 0.0f;
}

void UPREffect::UpdateEffect(float DeltaTime)
{
}

bool UPREffect::IsActivate() const
{
	return bActivate;
}

void UPREffect::Activate()
{
	bActivate = true;
	// Effect의 수명이 끝나면 Effect를 비활성화합니다.
	if(GetEffectOwner() != nullptr)
	{
		GetEffectOwner()->GetWorldTimerManager().SetTimer(LifespanTimerHandle, this, &UPREffect::Deactivate, Lifespan, false);
	}
}

void UPREffect::Deactivate()
{
	bActivate = false;
	if(GetEffectOwner() != nullptr)
	{
		GetEffectOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);
	}
	OnEffectDeactivate.Broadcast(this);
}

void UPREffect::Destroy()
{
	// 생선된 이 클래스의 메모리를 해제합니다.
	ConditionalBeginDestroy();
}

void UPREffect::SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate)
{
}

void UPREffect::SpawnEffectAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate)
{
}

FVector UPREffect::GetEffectLocation() const
{
	return FVector::ZeroVector;
}

UFXSystemComponent* UPREffect::GetFXSystemComponent() const
{
	return nullptr;
}

bool UPREffect::IsLooping() const
{
	return false;
}

UFXSystemAsset* UPREffect::GetEffectSourceAsset() const
{
	return EffectSourceAsset;
}

void UPREffect::SetEffectSourceAsset(UFXSystemAsset* NewEffectSourceAsset)
{
	EffectSourceAsset = NewEffectSourceAsset;
}

AActor* UPREffect::GetEffectOwner() const
{
	return EffectOwner;
}

void UPREffect::SetEffectOwner(AActor* NewEffectOwner)
{
	EffectOwner = NewEffectOwner;
}

int32 UPREffect::GetPoolIndex() const
{
	return PoolIndex;
}

void UPREffect::SetPoolIndex(int32 NewPoolIndex)
{
	PoolIndex = NewPoolIndex;
}

float UPREffect::GetLifeSpan() const
{
	return Lifespan;
}

void UPREffect::SetLifespan(float NewLifespan)
{
	Lifespan = NewLifespan;
}
