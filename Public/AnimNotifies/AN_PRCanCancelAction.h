// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRCanCancelAction.generated.h"

/**
 * 캐릭터의 CanCancelAction(액션을 캔슬할 수 있는 상태)를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRCanCancelAction : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRCanCancelAction();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 캐릭터의 CanCancelAction(액션을 캔슬할 수 있는 상태)를 설정하는 변수입니다. */
	void SetCanCancelAction(USkeletalMeshComponent* MeshComp);

private:
	/** 캐릭터의 CanCancelAction(액션을 캔슬할 수 있는 상태)를 설정하는 변수입니다. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CanCacelAction", meta = (AllowPrivateAccess = "true"))
	bool bCanCancelAction;
};
