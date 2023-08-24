// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "AnimInstances/PRBaseAnimInstance.h"
#include "PRPlayerAnimInstance.generated.h"

/**
 * 플레이어 캐릭터가 사용하는 AnimInstance 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRPlayerAnimInstance : public UPRBaseAnimInstance
{
	GENERATED_BODY()

public:
	UPRPlayerAnimInstance();
	
protected:
	/** 캐릭터의 MovementInfo를 최신화하는 함수입니다. */
	virtual void UpdateMovementInfo() override;

private:
	/** 캐릭터의 InputComponent의 MoveForward 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementInfo", meta = (AllowPrivateAccess = "true"))
	float MoveForward;

	/** 캐릭터의 InputComponent의 MoveRight 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementInfo", meta = (AllowPrivateAccess = "true"))
	float MoveRight;
};
