// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetIsInvincible.generated.h"

/**
 * 캐릭터의 무적 상태를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetIsInvincible : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetIsInvincible();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 캐릭터가 무적 상태인지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetIsInvincible", meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible;
};
