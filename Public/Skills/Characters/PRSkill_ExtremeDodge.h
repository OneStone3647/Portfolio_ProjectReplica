// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRCooldownSkill.h"
#include "PRSkill_ExtremeDodge.generated.h"

/**
 * 캐릭터가 적의 공격을 알맞은 타이밍에 회피할 경우 TimeStop을 실행하는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_ExtremeDodge : public UPRCooldownSkill
{
	GENERATED_BODY()

public:
	UPRSkill_ExtremeDodge();

public:
	/**
	 * 스킬을 실행하는 함수입니다.
	 *
	 * @return 스킬이 정상적으로 실행됐을 경우 true를 반환합니다. 실행되지 않았을 경우 false를 반환합니다.
	 */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지 판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;
};
