// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enums/Enum_PRWeaponEquipPosition.h"
#include "ANS_PRExecuteAttackTrace.generated.h"

/**
 * 캐릭터가 발도한 무기의 ExecuteAttackTrace를 실행하는 AnimNotifyState 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRExecuteAttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRExecuteAttackTrace(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** 무기의 장착 위치입니다. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "WeaponEquipPosition")
	EPRWeaponEquipPosition WeaponEquipPosition;
};
