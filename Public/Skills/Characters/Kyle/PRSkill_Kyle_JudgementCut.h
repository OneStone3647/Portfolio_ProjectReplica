// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRCooldownSkill.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "PRSkill_Kyle_JudgementCut.generated.h"

/**
 * 캐릭터가 검을 빠르게 휘둘러 일정범위에 참격을 일으키는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_Kyle_JudgementCut : public UPRCooldownSkill
{
	GENERATED_BODY()

public:
	UPRSkill_Kyle_JudgementCut();

public:
	/** 스킬을 초기화하는 함수입니다. */
	virtual void InitializeSkill_Implementation() override;
	
	/**
	 * 스킬을 실행하는 함수입니다.
	 *
	 * @return 스킬이 정상적으로 실행됐을 경우 true를 반환합니다. 실행되지 않았을 경우 false를 반환합니다.
	 */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지 판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;

private:
	/** 일정 범위에 대미지를 가하는 오브젝트의 정보입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectInfo", meta = (AllowPrivateAccess = "true"))
	FPRPooledObjectInfo JudgementCutArea;

#pragma region Cooldown
public:
	/** 재사용 대기시간이 끝나고 실행하는 함수입니다. */
	virtual void EndCoolDown_Implementation() override;
#pragma endregion

#pragma region PRAnimMontage
private:
	/** 스킬 실행시 사용하는 PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage JudgementCutPRAnimMontage;

	/** JudgementCutPRAnimMontage의 ID입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	int32 JudgementCutPRAnimMontageID;
#pragma endregion 
};
