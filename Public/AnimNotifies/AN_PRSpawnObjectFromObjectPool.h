// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSpawnObjectFromObjectPool.generated.h"

/**
 * 방향을 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRSpawnLocation : uint8
{
	SpawnLocation_None					UMETA(DisplayName = "None"),
	SpawnLocation_Custom				UMETA(DisplayName = "Custom"),
	SpawnLocation_MainWeapon			UMETA(DisplayName = "MainWeapon"),
	SpawnLocation_SubWeapon				UMETA(DisplayName = "SubWeapon")
};

/**
 * ObjectPoolSystem을 가지고 있는 액터의 ObjectPool에서 Object를 Spawn하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSpawnObjectFromObjectPool : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSpawnObjectFromObjectPool();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	/** PooledObject)를 Spawn할 위치를 설정하는 함수입니다. */
	// void SetPooledObjectSpawnLocation(AActor* NewPooledObject);

private:
	/** ObjectSystem을 가지고 있는 액터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	AActor* ObjectPoolSystemOwner;

	/** Spawn할 PooledObject의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	FName SpawnPooledObjectName;

	/** PooledObject를 Spawn할 위치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	EPRSpawnLocation SpawnLocation;

	/** PooledObject를 직접 Spawn할 위치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true", EditCondition = "SpawnLocation == EPRSpawnLocation::SpawnLocation_Custom", EditConditionHides))
	FVector SpawnCustomLocation;
};
