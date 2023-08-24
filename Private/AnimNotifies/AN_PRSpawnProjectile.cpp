// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSpawnProjectile.h"

UAN_PRSpawnProjectile::UAN_PRSpawnProjectile()
	: Super()
{
	bIsNativeBranchingPoint = true;

	// Projectile
	Projectile = nullptr;
}

void UAN_PRSpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();
	if(Owner != nullptr)
	{
		AActor* SpawnProjectile = Owner->GetWorld()->SpawnActor<AActor>(Projectile, Owner->GetActorTransform());
	}
}
