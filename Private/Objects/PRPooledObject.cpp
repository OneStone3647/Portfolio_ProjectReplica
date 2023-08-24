// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRPooledObject.h"

APRPooledObject::APRPooledObject()
{
	PrimaryActorTick.bCanEverTick = false;

	bActivate = false;
	ObjectOwner = nullptr;
	ObjectName = FName();
	Lifespan = 0.0f;
	PoolIndex = 0;
}

void APRPooledObject::BeginPlay()
{
	Super::BeginPlay();
	
}

bool APRPooledObject::IsActivate() const
{
	return bActivate;
}

void APRPooledObject::SetActivate_Implementation(bool bIsActivate)
{
	bActivate = bIsActivate;
	if(bIsActivate)
	{
		InitializeSpawnLocation();
	}
	SetActorHiddenInGame(!bIsActivate);
	
	// 오브젝트의 수명이 끝나면 오브젝트를 비활성화합니다.
	GetWorldTimerManager().SetTimer(LifespanTimerHandle, this, &APRPooledObject::Deactivate, Lifespan, false);
}

void APRPooledObject::InitializePooledObject_Implementation()
{
}

void APRPooledObject::Deactivate()
{
	SetActivate(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledObjectDeactivate.Broadcast(this);
}

void APRPooledObject::InitializeSpawnLocation_Implementation()
{
}

void APRPooledObject::SetLifespan(float NewLifespan)
{
	Lifespan = NewLifespan;
}

AActor* APRPooledObject::GetObjectOwner() const
{
	return ObjectOwner;
}

void APRPooledObject::SetObjectOwner(AActor* NewObjectOwner)
{
	ObjectOwner = NewObjectOwner;
}

FName APRPooledObject::GetObjectName() const
{
	return ObjectName;
}

void APRPooledObject::SetObjectName(FName NewObjectName)
{
	ObjectName = NewObjectName;
}

int32 APRPooledObject::GetPoolIndex() const
{
	return PoolIndex;
}

void APRPooledObject::SetPoolIndex(int32 NewPoolIndex)
{
	PoolIndex = NewPoolIndex;
}

