// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Skills/PRBaseSkill.h"
#include "ANS_PRSetActivateSkill.generated.h"

/**
 * 캐릭터의 스킬을 활성화/비활성화하는 AnimNotifyState 클래스입니다.
 * bActivate를 true로 설정할 경우 NotifyBegin에서 스킬을 활성화하고 NotifyEnd에서 스킬을 비활성화합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRSetActivateSkill : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRSetActivateSkill();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 캐릭터의 스킬을 활성화하는 함수입니다. */
	void SetActivateSkill(USkeletalMeshComponent* MeshComp, EPRCommandSkill NewCommandSkill, bool bNewActivate);

private:
	/** 활성화할 스킬의 커맨드입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActivateSkill", meta = (AllowPrivateAccess = "true"))
	EPRCommandSkill CommandSkill;

	/** 스킬의 활성화/비활성화를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActivateSkill", meta = (AllowPrivateAccess = "true"))
	bool bActivate;
	
};
