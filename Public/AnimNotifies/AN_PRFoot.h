// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRFoot.generated.h"

/**
 * 캐릭터의 발을 나타내는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRFoot : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRFoot();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 캐릭터가 내린 발입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRFoot", meta = (AllowPrivateAccess = "true"))
	EPRFoot FootDown;
};
