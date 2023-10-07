// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRTimedParticleEffect.generated.h"

class UParticleSystem;
class UPRParticleEffect;

/**
 * ParticleEffect를 일정 시간 활성화하고 종료시 비활성화하는 AnimNotifyState 클래스입니다.
 * NotifyBegin에서 ParticleEffect를 활성화하고 NotifyEnd에서 비활성화합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRTimedParticleEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRTimedParticleEffect();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** Template를 바탕으로 하는 ParticleEffect를 Spawn하고 MeshComp에 부착하는 함수입니다. */
	void SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

private:
	/** 생성할 ParticleSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* Template;

	/** Spawn한 PRParticleEffect입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	UPRParticleEffect* SpawnedPRParticleEffect;

	/** Spawn한 ParticleEffect입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SpawnedParticleEffect;

	/** 생성한 ParticleEffect를 부착할 Mesh의 소켓이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	/** 생성하는 ParticleEffect의 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	FVector LocationOffset;

	/** 생성하는 ParticleEffect의 회전 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	FRotator RotationOffset;

	/** 생성하는 ParticleEffect의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	FVector Scale;

	/** NotifyEnd에서 ParticleEffect를 제거할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedParticleEffect", meta = (AllowPrivateAccess = "true"))
	bool bDestroyAtEnd;
};
