// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetAerial.generated.h"

/**
 * 캐릭터의 공중에 머무는 상태(에어리얼)을 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetAerial : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetAerial();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 에어리얼 상태로 설정할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAerial", Meta = (AllowPrivateAccess = "true"))
	bool bIsAerial;
};
