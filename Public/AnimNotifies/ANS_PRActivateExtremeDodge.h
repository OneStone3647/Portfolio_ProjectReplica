// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRActivateExtremeDodge.generated.h"

class APRPlayerCharacter;

/**
 * 캐릭터의 극한 회피를 실행하는 AnimNotifyState 클래스입니다.
 * NotifyBegin을 실행했을 때 캐릭터의 위치와 NotifyTick을 실행하면서 이동한 캐릭터의 위치에 MultiTrace를 실행하여
 * 캐릭터가 극한 회피를 실행할 수 있는 상태일 경우 극한 회피를 실행합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRActivateExtremeDodge : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRActivateExtremeDodge();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** Debug를 실행하는 함수입니다. */
	void ActivateDebug();

private:
	/** 극한 회피를 실행하는 플레이어 캐릭터입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "AcitvateExtremeDodge", meta = (AllowPrivateAccess = "true"))
	APRPlayerCharacter* OwnerPRPlayerCharacter;

	/** Debug의 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AcitvateExtremeDodge", meta = (AllowPrivateAccess = "true"))
	bool bActivateDebug;

	/** Debug의 지속시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AcitvateExtremeDodge", meta = (AllowPrivateAccess = "true"))
	float DebugDuration;	
};
