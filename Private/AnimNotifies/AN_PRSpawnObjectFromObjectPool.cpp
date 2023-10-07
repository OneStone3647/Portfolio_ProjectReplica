// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSpawnObjectFromObjectPool.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Objects/PRPooledObject.h"

UAN_PRSpawnObjectFromObjectPool::UAN_PRSpawnObjectFromObjectPool()
	: Super()
{
	bIsNativeBranchingPoint = true;

	// ObjectPoolSystem
	ObjectPoolSystemOwner = nullptr;
	SpawnPooledObjectName = FName();
	SpawnLocation = EPRSpawnLocation::SpawnLocation_None;
	SpawnCustomLocation = FVector::ZeroVector;
}

void UAN_PRSpawnObjectFromObjectPool::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ObjectPoolSystemOwner = MeshComp->GetOwner();
	if(ObjectPoolSystemOwner != nullptr)
	{
		UPRObjectPoolSystemComponent* ObjectPoolSystem = ObjectPoolSystemOwner->FindComponentByClass<UPRObjectPoolSystemComponent>();
		if(ObjectPoolSystem != nullptr)
		{
			ObjectPoolSystem->ActivatePooledObject(SpawnPooledObjectName);
		}
	}
}
