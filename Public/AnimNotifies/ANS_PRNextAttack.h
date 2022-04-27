// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRNextAttack.generated.h"

/**
 * 캐릭터의 공격 입력에 따라 다음 공격으로 공격할지 공격을 초기화할지 결정하는 AnimNotifyState 클래스입니다.
 * AnimNotifyState에 진입하게 되면 캐릭터의 공격 입력을 받을 수 있게 캐릭터의 StateSystem의 OnAttack을 false로 변경하고
 * 캐릭터의 공격 입력을 받아 캐릭터의 StateSystem의 OnAttack이 true가 되었을 경우 캐릭터의 무기의 WeaponPRAnimMontageIndex를 증가 시킵니다.
 * 캐릭터의 공격 입력이 없이 AnimNotifyState를 벗어나게 되면 캐릭터의 무기의 WeaponPRAnimMontageIndex을 0으로 초기화합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRNextAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UANS_PRNextAttack(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
