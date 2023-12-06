// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PROnSkill.generated.h"

class UPRBaseSkill;

/**
 * 스킬의 OnSkillNotifyState 함수를 실행하는 AnimNotifyState 클래스입니다.
 * 스킬을 실행하고 재생되는 애니메이션의 특정 구간동안 스킬의 함수를 실행할 때 사용합니다. 
 */
UCLASS()
class PROJECTREPLICA_API UANS_PROnSkill : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PROnSkill();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	/**
	 * 이 AnimNotifyState를 실행하는 캐릭터의 SkillInventory에 ActivateSkill이 존재한다면
	 * ActivateSkill의 OnSkillBegin 함수를 실행하는 함수입니다.
	 */
	void OnSkillBegin(USkeletalMeshComponent* MeshComp);

	/**
	 * 이 AnimNotifyState를 실행하는 캐릭터의 SkillInventory에 ActivateSkill이 존재한다면
	 * ActivateSkill의 OnSkillTick 함수를 실행하는 함수입니다.
	 */
	void OnSkillTick(USkeletalMeshComponent* MeshComp, float DeltaTime);

	/**
	 * 이 AnimNotifyState를 실행하는 캐릭터의 SkillInventory에 ActivateSkill이 존재한다면
	 * ActivateSkill의 OnSkillEnd 함수를 실행하는 함수입니다.
	 */
	void OnSkillEnd(USkeletalMeshComponent* MeshComp);

private:
	/** 실행할 스킬입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PROnSkill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPRBaseSkill> ActivateSkill;

	
};
