// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRFootstep.generated.h"

/**
 * 발소리를 재생하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRFootstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRFootstep();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	/** 발소리를 재생하는 함수입니다. */
	void PlayFootstep(USkeletalMeshComponent* MeshComp);

private:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebug;
	
	/** 재생할 발소리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep", meta = (AllowPrivateAccess = "true"))
	USoundBase* FootstepSound;

	/** 사운드를 더 크게, 또는 더 부드럽게 만들기 위해 볼륨에 곱하는 단방향 스칼라입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep", meta = (AllowPrivateAccess = "true"))
	float VolumeMultiplier;

	/** 피치에 곱하는 단방향 스칼라입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep", meta = (AllowPrivateAccess = "true"))
	float PitchMultiplier;

	/** 발바닥에서부터 발소리를 출력할 피직스 머테리얼을 탐색하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep", meta = (AllowPrivateAccess = "true"))
	float CheckDistance;

	/** Footstep의 재생을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootstep", meta = (AllowPrivateAccess = "true"))
	bool bPlayFootstep;
};
