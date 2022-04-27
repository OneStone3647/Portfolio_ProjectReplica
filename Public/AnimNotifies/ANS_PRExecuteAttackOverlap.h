// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRExecuteAttackOverlap.generated.h"

/**
 * 캐릭터에 설정한 OverlapCollision의 ExecuteAttackOverlap을 실행하는 AnimNotifyState 클래스입니다.
 * NotifyBegin에서 OverlapEvent를 활성화하고 NotifyEnd에서 비활성화 합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRExecuteAttackOverlap : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRExecuteAttackOverlap(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	
private:
	/** Overlap에 사용하는 OverlapCollision입니다. */
	UPROPERTY()
	class APROverlapCollision* AttackOverlapCollision;

	/** Overlap의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OverlapCollision", meta = (AllowPrivateAccess = "true"))
	FVector OverlapSize;
};
