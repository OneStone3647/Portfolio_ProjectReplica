// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRActivateAerial.generated.h"

/**
 * 캐릭터의 공중에서 머무는(에어리얼) 상태를 실행하는 AnimNotify 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRActivateAerial : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRActivateAerial();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	void ActivateAerial(USkeletalMeshComponent* MeshComp);
	
private:
	/** 캐릭터를 공중에 머무는(에어리얼) 상태로 설정할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRExecuteAerial", meta = (AllowPrivateAccess = "true"))
	bool bActivateAerial;
};
