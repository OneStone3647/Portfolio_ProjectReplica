// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Enums/Enum_PRJump.h"
#include "AN_PRJump.generated.h"

/**
 * 점프를 시작할 때와 끝날 때의 소리를 선택하여 출력하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRJump : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRJump();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 점프를 구분하는 열거형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRJump", Meta = (AllowPrivateAccess = "true"))
	EPRJump Jump;

	/** 발소리를 출력할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRJump", Meta = (AllowPrivateAccess = "true"))
	bool bPlayFootStep;

	/** 목소리를 출력할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRJump", Meta = (AllowPrivateAccess = "true"))
	bool bPlayVoice;
};
