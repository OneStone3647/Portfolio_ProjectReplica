// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRBaseSkill.h"
#include "PRCooldownSkill.generated.h"

/**
 * 쿨타임을 가지는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRCooldownSkill : public UPRBaseSkill
{
	GENERATED_BODY()

public:
	/**
	 * 스킬을 실행하는 함수입니다.
	 *
	 * @return 스킬이 정상적으로 실행됐을 경우 true를 반환합니다. 실행되지 않았을 경우 false를 반환합니다.
	 */
	virtual bool ActivateSkill_Implementation() override;

#pragma region Cooldown
public:
	/** 재사용 대기시간을 실행하는 함수입니다. */
	virtual void ActivateCooldown_Implementation() override;

	/** 재사용 대기시간이 실행되고 있는지 판별하는 함수입니다. */
	virtual bool IsCooldown() const override;

	/** 재사용 대기시간의 남은 시간을 반환하는 함수입니다. */
	virtual float GetSkillCooldownRemaining() const override;

	/** 재사용 대기시간의 남은 시간의 비율을 반환하는 함수입니다. */
	virtual float GetSkillCooldownRemainingRatio() const override;

	/** 재사용 대기시간의 경과 시간을 반환하는 함수입니다. */
	virtual float GetSkillCooldownElapsed() const override;

	/** 재사용 대기시간의 경과 시간의 비율을 반환하는 함수입니다. */
	virtual float GetSkillCooldownElapsedRatio() const override;

protected:
	/** 재사용 대기시간이 끝나고 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	void EndCoolDown();
	virtual void EndCoolDown_Implementation();

protected:
	/** 재사용 대기시간에 사용하는 TimerHandle입니다. */
	FTimerHandle CooldownTimerHandle;
#pragma endregion
};
