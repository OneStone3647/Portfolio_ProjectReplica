// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PROnSkill.generated.h"

class UPRBaseSkill;

/**
 * 스킬의 OnSkillNotify 함수를 실행하는 AnimNotify 클래스입니다.
 * 스킬을 실행하고 재생되는 애니메이션의 특정 타이밍에 스킬의 함수를 실행할 때 사용합니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PROnSkill : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PROnSkill();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	/** 이 AnimNotify를 실행하는 캐릭터의 SkillSystem에서 ActivateSkill의 OnSkillNotify 함수를 실행하는 함수입니다. */
	void OnSkill(USkeletalMeshComponent* MeshComp);

private:
	/** 실행할 스킬입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PROnSkill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPRBaseSkill> ActivateSkill;
};
