// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimInstance.h"
#include "Common/PRCommonEnum.h"
#include "PRBaseAnimInstance.generated.h"

/**
 * 캐릭터가 사용하는 AnimInstance의 기본이 되는 AnimInstance 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPRBaseAnimInstance();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma region CharacterReference
protected:
	/** PROwner를 초기화하는 함수입니다. */
	void InitializePROwner();
	
protected:
	/** 이 AnimInstance를 사용하는 Pawn을 PRBaseCharacter 클래스로 캐스팅한 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseAnimInstance|CharacterReference")
	class APRBaseCharacter* PROwner;
#pragma endregion

#pragma region CharacterState
protected:
	/** 캐릭터의 상태를 업데이트하는 함수입니다. */
	void UpdateCharacterState();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterState")
	bool bIsDead;
#pragma endregion 

#pragma region MovementInfo
protected:
	/** 캐릭터의 MovementInfo를 최신화하는 함수입니다. */
	virtual void UpdateMovementInfo();

	/**
	 * 입력받은 인자가 MovementState와 같은지 판별하는 함수입니다.
	 * @param NewPRMovementState MovementState와 같은지 판별할 PRMovementState입니다.
	 * @return 같은 경우 true, 다를 경우 false를 반환합니다. 
	 */
	bool IsEqualMovementState(EPRMovementState NewPRMovementState) const;

protected:
	/** 캐릭터의 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRMovementState MovementState;

	/** 캐릭터의 이전 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRMovementState PreviousMovementState;

	/** 캐릭터가 점프했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	bool bJumped;
	
	/** 캐릭터가 내린 발입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "MovementInfo")
	EPRFoot FootDown;

	/** 캐릭터의 허용된 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait AllowedGait;

	/** 캐릭터의 이전 허용된 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait PreviousAllowedGait;
	
	/** 캐릭터의 현재 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait Gait;

	/** 캐릭터의 이전 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait PreviousGait;

	/** 캐릭터의 최대 걷기 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	float MaxWalkSpeed;

	/** 캐릭터의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	FVector Velocity;
	
	/** 캐릭터가 멈췄을 때의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float SpeedWhenStop;
	
	/** 캐릭터가 점프했을 때의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float SpeedWhenJump;
	
	/** 캐릭터가 움직이는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	bool bIsMoving;

	/** 캐릭터의 현재 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	FVector CurrentAcceleration;

public:
	/** 입력받은 인자로 FootDown을 설정하는 함수입니다. */
	void SetFootDown(EPRFoot NewFoot);

	/**
	 * 입력받은 인자로 AllowedGait를 설정하는 함수입니다.
	 * 입력받은 인자로 AllowedGait를 설정하기 전에 AllowedGait를 PreviousAllowedGait에 저장합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementInfo")
	void SetAllowedGait(EPRGait NewAllowedGait);

	/**
	 * 입력받은 인자로 Gait를 설정하는 함수입니다.
	 * 입력받은 인자로 Gait를 설정하기 전에 Gait를 PreviousGait에 저장합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementInfo")
	void SetGait(EPRGait NewGait);
#pragma endregion

#pragma region FootIK
protected:
	/** FootIK의 값을 초기화하는 함수입니다. */
	void InitializeFootIK();

	/** FootIK 디버그를 최신화하는 함수입니다. */
	void UpdateFootIKDebug();

	/**
	 * FootIk를 최신화하는 함수입니다.
	 * @param DeltaTime 프레임 사이에 경과된 시간입니다. 월드의 DeltaSeconds나 NativeUpdateAnimation함수의 DeltaSeconds를 사용합니다.
	 */
	void UpdateFootIK(float DeltaTime);

	/**
	 * SocketName에 해당하는 Socket의 위치에서 FootIKTrace를 실행하고 최신화하는 함수입니다.
	 * @param HitResult Trace의 Hit결과를 나타내는 FHitResult 구조체입니다.
	 * @param FootIKOffset FootIK의 Offset입니다.
	 * @param SocketName Trace할 Socket의 이름입니다.
	 * @param bDebugFlag 디버그의 출력을 결정하는 변수입니다.
	 * @param TraceDistance Trace하는 거리입니다.
	 * @param TraceRadius Trace하는 Sphere의 반지름입니다.
	 * @return Trace의 블록 여부입니다.
	 */
	bool UpdateFootIKTrace(FHitResult& HitResult, float& FootIKOffset, FName SocketName, bool bDebugFlag, float TraceDistance = 60.0f, float TraceRadius = 6.0f);
	
	/**
	 * FootIK의 Offset 값을 보간하여 최신화하는 함수입니다.
	 * @param TargetOffset 보간할 목표 Offset입니다.
	 * @param CurrentOffset 현재 Offset입니다
	 * @param DeltaTime 프레임 사이에 경과된 시간입니다. 월드의 DeltaSeconds나 NativeUpdateAnimation함수의 DeltaSeconds를 사용합니다.
	 * @param InterpSpeed 보간하는 속도입니다.
	 */
	void UpdateFootIKOffset(float TargetOffset, float& CurrentOffset, float DeltaTime, float InterpSpeed);
	
	/**
	 * FootIK의 Rotation 값을 보간하여 최신화하는 함수입니다.
	 * @param TargetRotation 보간할 목표 Rotation입니다.
	 * @param CurrentRotation 현재 Rotation입니다.
 	 * @param DeltaTime 프레임 사이에 경과된 시간입니다. 월드의 DeltaSeconds나 NativeUpdateAnimation함수의 DeltaSeconds를 사용합니다.
	 * @param InterpSpeed 보간하는 속도입니다.
	 */
	void UpdateFootIKRotation(FRotator TargetRotation, FRotator& CurrentRotation, float DeltaTime, float InterpSpeed);

	/**
	 * HipsOffset 값에 따라 캐릭터의 CapsuleCollision의 HalfHeight 값을 보간하여 최신화하는 함수입니다.
	 * @param CurrentHipsOffset 현재 HipsOffset입니다.
	 * @param DeltaTime 프레임 사이에 경과된 시간입니다. 월드의 DeltaSeconds나 NativeUpdateAnimation함수의 DeltaSeconds를 사용합니다.
	 * @param bInitializeFlag 캐릭터의 CapsuleCollision의 HalfHeight 값을 초기화할지 결정하는 변수입니다.
	 */
	void UpdateCapsuleHalfHeight(float CurrentHipsOffset, float DeltaTime, bool bInitializeFlag = false);
	
	/**
	 * 법선 벡터로 회전값을 계산하는 함수입니다.
	 * @param NormalVector 회전값으로 계산할 법선 벡터입니다.
	 * @return 법선 벡터로 계산한 회전값입니다.
	 */
	FRotator CalculateNormalToRotator(FVector NormalVector);
	
protected:
	/** FootIK 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIK")
	bool bActiveFootIK;
	
	/** FootIK 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIk")
	bool bActiveFootIKDebug;
	
	/** FootIK에 사용하는 Trace 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIk")
	float FootIKTraceDistance;

	/** FootIK에 사용하는 SphereTrace의 반지름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIk")
	float FootIKTraceRadius;
	
	/** 왼발의 본 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FootIk")
	FName LeftFootBoneName;
	
	/** 오른발의 본 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FootIk")
	FName RightFootBoneName;

	/** 바닥과 발 사이의 Offset(Z) 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FootIk")
	float AdjustFootOffset;
	
	/** Foot의 보간 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIk")
	float FootInterpSpeed;

	/** HipsOffset의 보간 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootIk")
	float HipsOffsetInterpSpeed;

	/** 왼쪽 발 이펙터의 Offset(X) 값입니다. */
	float LeftFootEffectorLocationOffset;

	/** 오른쪽 발 이펙터의 Offset(X) 값입니다. */
	float RightFootEffectorLocationOffset;

	/** 왼쪽 발 이펙터의 위치 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FVector LeftFootEffectorLocation;
	
	/** 오른쪽 발 이펙터의 위치 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FVector RightFootEffectorLocation;

	/** 엉덩이(pelvis)의 Offset(Z) 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	float HipsOffset;

	/** PROwner의 CapsuleComponent의 초기 HalfHeight입니다. */
	float OwnerInitializeCapsuleHalfHeight;

	/** 왼쪽 발의 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FRotator LeftFootRotation;

	/** 오른쪽 발의 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FRotator RightFootRotation;

	/** 왼쪽 조인트 타겟의 위치입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FVector JointTargetLeft;

	/** 오른쪽 조인트 타겟의 위치입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FootIk")
	FVector JointTargetRight;
	
public:
	/** 입력받은 인자로 bActiveFootIK를 설정하는 함수입니다. */
	void SetActiveFootIK(bool bFlag);
#pragma endregion 
};
