// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetActionable.generated.h"

/**
 * 캐릭터의 ActionState를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetActionable : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetActionable();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/**
	 * 캐릭터의 ActionState를 설정하는 함수입니다.
	 *
	 * @param MeshComp AnimNotify를 사용하는 Mesh입니다.
	 * @param Action 액션의 종류입니다.
	 * @param bAble 액션 가능 상태입니다.
	 */
	void SetActionable(USkeletalMeshComponent* MeshComp, EPRAction Action, bool bAble);
	
private:
	/**
	  * 캐릭터의 액션 가능 상태를 나타내는 Map입니다.
	  *
	  * @Key 액션의 종류입니다.
	  * @Value 액션 가능 상태입니다. 액션이 실행 가능한 상태일 경우 true, 실행 불가능한 상태일 경우 false를 나타냅니다.
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionState", meta = (AllowPrivateAccess = "true"))
	TMap<EPRAction, bool> ActionStates;
};
