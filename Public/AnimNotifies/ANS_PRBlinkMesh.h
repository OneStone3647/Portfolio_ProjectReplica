// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRBlinkMesh.generated.h"

/**
 * 캐릭터의 Mesh를 숨기고 다시 나타내는 AnimNotifyState 클래스입니다.
 * NotifyBegin에서 Mesh를 숨기고 NotifyEnd에서 다시 나타냅니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRBlinkMesh : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRBlinkMesh();
	
protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	/** true일 경우 Mesh를 나타낸 후 숨깁니다. false일 경우 Mesh를 숨기고 다시 나타냅니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlinkMesh", meta = (AllowPrivateAccess = "true"))
	bool bBlinkReverse;
};
