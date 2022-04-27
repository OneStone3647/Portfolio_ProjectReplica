// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRFootStep.generated.h"

/**
 * 발소리를 출력하는 AnimNotify 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRFootStep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAN_PRFootStep();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	/** 발소리 출력 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFootStep", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FootStepVolume;
};
