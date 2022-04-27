// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetCharacterState.generated.h"

/**
 * 캐릭터의 State를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetCharacterState : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetCharacterState();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = true))
	bool bIsHit;
};
