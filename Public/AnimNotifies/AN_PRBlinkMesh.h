// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRBlinkMesh.generated.h"

/**
 * 캐릭터의 매쉬를 감추거나 다시 나타나게 하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRBlinkMesh : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRBlinkMesh();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "BlinkMesh", meta = (AllowPrivateAccess = "true"))
	bool bBlinkMesh;
};
