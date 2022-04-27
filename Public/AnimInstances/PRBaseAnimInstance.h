// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimInstance.h"
#include "Enums/Enum_PRMovementState.h"
#include "Enums/Enum_PRGait.h"
#include "PRBaseAnimInstance.generated.h"

/**
 * 캐릭터가 사용하는 AnimInstance의 바탕이 되는 AnimInstance 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPRBaseAnimInstance();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma region CharacterReference
protected:
	/** 이 AnimInstance를 사용하는 Pawn을 PRBaseCharacter 클래스로 캐스팅한 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance|CharacterReference")
	class APRBaseCharacter* PROwner;
#pragma endregion

#pragma region MovementInfo
protected:
	/** 캐릭터의 움직임 정보를 업데이트하는 함수입니다. */
	void UpdateMovementInfo();

	/** 움직임 애니메이션의 재생속도를 계산하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseAnimInstance|MovementInfo")
	float CalculateMovePlayRate() const;
	
protected:
	/** 캐릭터의 움직임 상태입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	EPRMovementState MovementState;

	/** 캐릭터의 이전 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	EPRMovementState PreviousMovementState;

	/** 캐릭터의 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	EPRGait Gait;
	
	/** 캐릭터의 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float Speed;

	/** 캐릭터의 걷기 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float WalkSpeed;

	/** 걷기 애니메이션의 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float AnimatedWalkSpeed;

	/** 달리기 애니메이션의 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float AnimatedRunSpeed;

	/** 전력질주 애니메이션의 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float AnimatedSprintSpeed;

	/** 캐릭터가 움직이고 있는지 나타내는 변수입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	bool bIsMoving;

	/** 캐릭터가 가속 상태인지 나타내는 변수입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	bool bIsAccelerating;

	/** 멈췄을 때의 속도입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float SpeedWhenStopping;

	/** 움직임 애니메이션의 재생속도 입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float MovePlayRate;

	/** 캐릭터가 이동하는 방향입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRBaseAnimInstance|MovementInfo")
	float Direction;
#pragma endregion

#pragma region CharacterState
protected:
	/** 캐릭터의 상태를 업데이트하는 함수입니다. */
	void UpdateCharacterState();

protected:
	/** 캐릭터가 사망했는지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseAnimInstance|CharacterState")
	bool bIsDead;
#pragma endregion 
};
