// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetMeshVisibility.generated.h"

/**
 * 캐릭터의 Mesh의 Visibility를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetMeshVisibility : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetMeshVisibility();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** Mesh의 Visibility를 설정하는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetMeshVisibility", meta = (AllowPrivateAccess = "true"))
	bool bSetMeshVisibility;
};
