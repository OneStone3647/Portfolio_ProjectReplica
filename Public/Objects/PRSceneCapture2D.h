// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/SceneCapture2D.h"
#include "PRSceneCapture2D.generated.h"

class UDestructibleComponent;

/**
 * 화면을 캡쳐하는 SceneCapture2D 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRSceneCapture2D : public ASceneCapture2D
{
	GENERATED_BODY()

public:
	APRSceneCapture2D();

protected:
	virtual void BeginPlay() override;

private:
	/** 화면의 깨짐을 표현하는 DestructibleComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	UDestructibleComponent* Destructible;

	/** 화면과 Destructible을 합성할 크로마키입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ChromaKey;

	/** Destructible에 줄 대미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	/** 대미지를 입힐 중심점의 위치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	FVector HurtOrigin;

	/** 대미지가 적용될 반경입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	float DamageRadius;

	/** 파괴 가능한 오브젝트에 가해질 타격의 세기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	float ImpulseStrength;

	/** 파괴 가능한 오브젝트의 중심에서 피해 반경 끝까지의 피해량이 증가하는지 여부를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	bool bFullDamage;

	/** 중력을 적용하기 전까지의 딜레이 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	float EnableGravityDelay;

	/** 이 액터를 소멸시키기 전까지의 딜레이 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible", meta = (AllowPrivateAccess = "true"))
	float DestroyDelay;

public:
	/** Destructible을 반환하는 함수입니다. */
	class UDestructibleComponent* GetDestructible() const { return Destructible; }

	/** ChromaKey를 반환하는 함수입니다. */
	class UStaticMeshComponent* GetChromaKey() const { return ChromaKey; }
};
