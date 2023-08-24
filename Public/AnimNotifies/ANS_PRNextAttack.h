// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRNextAttack.generated.h"

/**
 * 캐릭터의 공격 입력에 따라 다음 공격으로 공격할지 공격을 초기화할지 결정하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRNextAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRNextAttack();

protected:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
