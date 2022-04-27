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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma region CharacterReference
protected:
	/** 이 AnimInstance를 사용하는 Pawn을 PRPlayerCharacter 클래스로 캐스팅한 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterReference")
	class APRPlayerCharacter* PRPlayerOwner;
#pragma endregion 

#pragma region MovementInfo
protected:
	/** 플레이어 캐릭터의 움직임 정보를 업데이트하는 함수입니다. */
	void UpdatePlayerMovementInfo();

protected:
	/** DoubleJump인지 나타내는 변수입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "CharacterInfo")
	bool bIsDoubleJump;

	/** Target을 LockOn 중인지 나타내는 변수입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "CharacterInfo")
	bool bIsLockOnTarget;

	/** 공중에서 머무는(에어리얼) 상태였는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo")
	bool bWasAerial;
#pragma endregion
};
