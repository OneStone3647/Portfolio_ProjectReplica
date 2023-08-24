// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Skills/PRBaseSkill.h"
#include "AN_PRSetActivateSkill.generated.h"

/**
 * 캐릭터의 스킬을 활성화/비활성화하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetActivateSkill : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetActivateSkill();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
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
