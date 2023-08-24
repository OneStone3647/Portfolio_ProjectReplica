// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/PRBaseSkill.h"
#include "PRNoneCooldownSkill.generated.h"

/**
 * 재사용 대기시간을 가지지 않는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRNoneCooldownSkill : public UPRBaseSkill
{
	GENERATED_BODY()

public:
	virtual bool ActivateSkill_Implementation() override;
};
