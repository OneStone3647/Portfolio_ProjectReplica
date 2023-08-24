// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetActiveFootIK.generated.h"

/**
 * FootIK의 실행을 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetActiveFootIK : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetActiveFootIK();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** FootIK를 활성화하는 함수입니다. */
	void ActiveFootIK(USkeletalMeshComponent* MeshComp);

private:
	/** FootIK의 활성화를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRSetActiveFootIK", meta = (AllowPrivateAccess = "true"))
	bool bActive;
};
