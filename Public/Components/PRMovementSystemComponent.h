// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Components/TimelineComponent.h"
#include "PRMovementSystemComponent.generated.h"

/**
 * 캐릭터의 회전 방식을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRRotationMode : uint8
{
	RotationMode_VelocityDirection		UMETA(DisplayName = "VelocityDirection"),
	RotationMode_LookingDirection		UMETA(DisplayName = "LookingDirection")
};

/**
 * 캐릭터의 움직임을 관리하는 ActorComponent입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRMovementSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:	
	UPRMovementSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region DistanceMatching
public:
	/**
	 * 멈추는 위치를 예측하는 함수입니다.
	 * @param OutStopLocation			멈추는 위치입니다.
	 * @param CurrentLocation			현재 위치입니다.
	 * @param Velocity					속도입니다.
	 * @param Acceleration				현재 가속도입니다.
	 * @param Friction					마찰입니다.
	 * @param BrakingDeceleration		가속을 적용하지 않을 때 일어나는 제동 감속입니다.
	 * @param TimeStep					시간입니다.
	 * @param MaxSimulationIterations	최대 반복 횟수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMovementSystem|DistanceMatching")
	static bool PredictStopLocation(FVector& OutStopLocation, const FVector& CurrentLocation, const FVector& Velocity, const FVector& Acceleration, float Friction, float BrakingDeceleration, const float TimeStep, const int32 MaxSimulationIterations = 100);

	/**
	 * 애니메이션이 사용하는 Curve의 시간을 반환하는 함수입니다.
	 * @param AnimationSequence		Curve의 시간을 가져올 Curve를 사용하는 AnimSequence입니다.
	 * @param CurveName				Curve의 시간을 가져올 Curve의 이름입니다.
	 * @param CurveValue			Curve의 값입니다.
	 */
	UFUNCTION(BlueprintPure, Category = "PRMovementSystem|DistanceMatching")
	static float GetCurveTime(const UAnimSequence* AnimationSequence, const FName CurveName, const float CurveValue);
#pragma endregion 

#pragma region MovementInfo
public:
	/**
	 * 캐릭터의 OnMovementModeChanged 함수에서 사용하여 MovementState를 설정하는 함수입니다.
	 * 
	 * @param NewMovementMode MovementState의 값을 결정하는 MovementMode입니다. 
	 */
	void OnCharacterMovementModeChanged(EMovementMode NewMovementMode);

	/**
	 * 입력받은 인자가 MovementState와 같은지 판별하는 함수입니다.
	 * 
	 * @return 같은 경우 true, 다를 경우 false를 반환합니다. 
	 */
	bool IsEqualMovementState(const EPRMovementState& NewMovementState) const;

	/**
	 * 입력받은 인자가 AllowedGait와 같은지 판별하는 함수입니다.
	 *
	 * @return 같을 경우 true, 다를 경우 false를 반환합니다
	 */
	bool IsEqualAllowedGait(const EPRGait& NewAllowedGait) const;
	
	/**
	 * 입력받은 인자로 AllowedGait를 설정하는 함수입니다.
	 * 설정한 AllowedGait에 따라 Speed, Acceleration, BrakingDeceleration, BrakingFrictionFactor의 값을 변경합니다.
	 */
	void SetAllowedGait(EPRGait NewAllowedGait);

	/**
	 * 입력받은 인자가 Gait와 같은지 판별하는 함수입니다.
	 *
	 * @return 같을 경우 true, 다를 경우 false를 반환합니다
	 */
	bool IsEqualGait(const EPRGait& NewGait) const;
	
	/** 입력받은 속도에 따라 Gait를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRMovementSystem|MovementInfo")
	void UpdateGait(float NewCurrentSpeed);

	/**
	 * 입력받은 인자로 캐릭터의 회전 방식을 설정합니다.
	 *
	 * @param NewRotationMode 설정할 캐릭터의 회전 방식입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMovementSystem|MovementInfo")
	void SetRotationMode(EPRRotationMode NewRotationMode);

	/**
	 * 입력받은 걷는 모양에 해당하는 캐릭터의 MaxWalkSpeed를 반환하는 함수입니다.
	 * 
	 * @param NewGait 캐릭터의 걷는 모양입니다.
	 * @return NewGait에 해당하는 MaxWalkSpeed입니다.
	 */
	float GetMaxWalkSpeed(EPRGait NewGait);

	/**
	 * 캐릭터가 현재 움직이는지 판별하는 함수입니다.
	 * 
	 * @return 캐릭터가 움직이고 있으면 true를 반환합니다. 아닐 경우 false를 반환합니다.
	 */
	bool IsMoving() const;

	/**
	 * 캐릭터가 가속 상태인지 판별하는 함수입니다.
	 * 
	 * @return 캐릭터가 가속 상태면 true를 반환합니다. 아닐 경우 false를 반환합니다.
	 */
	bool IsAccelerating() const;

protected:
	/** Movement에 관련된 정보를 초기화하는 함수입니다. */
	void InitializeMovementInfo();
	
	/** Movement에 관련된 정보를 최신화하는 함수입니다. */
	void UpdateMovementInfo();

	/**
	 * MaxWalkSpeedTimeline을 초기화하는 함수입니다.
	 * Curve가 사용하는 Callback 함수에 사용할 함수를 바인드합니다.
	 * Timeline에 Curve를 추가합니다.
	 */
	void InitializeMaxWalkSpeedTimeline();

	/**
	 * MaxWalkSpeedTimeline에서 사용하는 MaxWalkSpeed를 변경하는 함수입니다.
	 * @param Value Curve에 의해 받는 값입니다.
	 */ 
	UFUNCTION()
	void ChangeMaxWalkSpeed(float Value);

	/** 캐릭터가 멈췄을 때의 속도를 설정하는 함수입니다. */
	void SetSpeedWhenStop(float& NewSpeedWhenStop);

	/** 캐릭터가 점프할 때의 속도를 설정하는 함수입니다. */
	void SetSpeedWhenJump(float& NewSpeedWhenJump);

	/** 캐릭터가 멈췄을 때의 속도로 캐릭터의 감속을 설정하는 함수입니다. */
	void SetBrakingInfo(float NewSpeedWhenStop);

protected:
	/** 캐릭터의 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRMovementState MovementState;

	/** 캐릭터의 이전 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRMovementState PreviousMovementState;

	/** 캐릭터의 허용된 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait AllowedGait;

	/** 캐릭터의 이전 허용된 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait PreviousAllowedGait;
	
	/** 캐릭터의 현재 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRGait Gait;

	/** 캐릭터의 회전 방식입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInfo")
	EPRRotationMode RotationMode;

	/** 캐릭터의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float CurrentSpeed;

	/** 캐릭터의 평면에서의 속도입니다.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float CurrentSpeed2D;

	/** 캐릭터가 멈출 때의 속도를 초기화했는지 나타내는 변수입니다. */
	bool bInitializeSpeedWhenStop;
	
	/** 캐릭터가 멈출 때의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float SpeedWhenStop;

	/** SpeedWhenJump를 초기화 때 딜레이를 주는 TimerHandle입니다. */
	FTimerHandle SpeedWhenJumpDelayTimerHandle;
	
	/** 캐릭터가 점프할 때의 속도를 초기화했는지 나타내는 변수입니다. */
	bool bInitializeSpeedWhenJump;
	
	/** 캐릭터가 점프할 때의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	float SpeedWhenJump;
	
	/** 캐릭터가 움직이는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	bool bIsMoving;

	/** 캐릭터의 현재 가속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementInfo")
	FVector CurrentAcceleration;
	
	/** 캐릭터의 걷기 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Walk")
	float WalkSpeed;

	/** 캐릭터가 걸을 때 최대 가속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Walk")
	float WalkAcceleration;

	/** 캐릭터가 걷고 가속을 적용하지 않을 때 일어나는 감속입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Walk")
	float WalkBrakingDeceleration;

	/** 캐릭터가 걸을 때 제동에 사용되는 실제 마찰 값을 얻기 위해 마찰값에 곱하는 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Walk")
	float WalkBrakingFrictionFactor;
	
	/** 캐릭터의 달리기 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Run")
	float RunSpeed;

	/** 캐릭터가 달릴 때 최대 가속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Run")
	float RunAcceleration;

	/** 캐릭터가 달리고 가속을 적용하지 않을 때 일어나는 감속입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Run")
	float RunBrakingDeceleration;

	/** 캐릭터가 달릴 때 제동에 사용되는 실제 마찰 값을 얻기 위해 마찰값에 곱하는 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Run")
	float RunBrakingFrictionFactor;

	/** 캐릭터의 전력 질주 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Sprint")
	float SprintSpeed;

	/** 캐릭터가 전력 질주할 때 최대 가속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Sprint")
	float SprintAcceleration;

	/** 캐릭터가 전력 질주하고 가속을 적용하지 않을 때 일어나는 감속입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Sprint")
	float SprintBrakingDeceleration;

	/** 캐릭터가 전력 질주할 때 제동에 사용되는 실제 마찰 값을 얻기 위해 마찰값에 곱하는 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementInfo|Sprint")
	float SprintBrakingFrictionFactor;

	/** 캐릭터의 MaxWalkSpeed를 설정할 때 사용하는 Timeline입니다. */
	FTimeline MaxWalkSpeedTimeline;

	/** MaxWalkSpeed 값을 변경할 Alpha 값을 가진 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementInfo")
	UCurveFloat* ChangeMaxWalkSpeedAlphaCurveFloat;

public:
	/** MovementState를 반환하는 함수입니다. */
	EPRMovementState GetMovementState() const;
	
	/**
	 * 입력받은 인자로 MovementState를 설정하는 함수입니다.
	 * MovementState를 설정하기 전 PreviousMovementState 값을 저장합니다.
	 * @param NewPRMovementState MovementState를 설정할 매개변수입니다.
	 */
	void SetMovementState(EPRMovementState NewPRMovementState);
	
	/** PreviousMovementState를 반환하는 함수입니다. */
	EPRMovementState GetPreviousMovementState() const;

	/** AllowedGait를 반환하는 함수입니다. */
	EPRGait GetAllowedGait() const;
	
	/** Gait를 반환하는 함수입니다. */
	EPRGait GetGait() const;

	/** CurrentSpeed를 반환하는 함수입니다. */
	float GetCurrentSpeed() const;

	/** SpeedWhenStop을 반환하는 함수입니다. */
	float GetSpeedWhenStop() const;

	/** 입력받은 인자로 bInitializeSpeedWhenJump를 설정하는 함수입니다. */
	void SetInitializeSpeedWhenJump(bool bFlag);

	/** SpeedWhenJump를 반환하는 함수입니다. */
	float GetSpeedWhenJump() const;
#pragma endregion

#pragma region GravityControl
public:
	/**
	 * 공중에서 머무는(에어리얼) 상태를 실행하는 함수입니다.
	 * @param bNewActiveAerial: true일 경우 이동을 중지(속도를 0으로 설정, 가속이 있는 구성요소의 경우 가속을 0으로 설정)하고
	 *							캐릭터의 GravityScale이 0.0f으로 설정합니다.
	 *							false일 경우 캐릭터의 GravityScale을 1.0f으로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "GravityControl")
	void ActivateAerial(bool bNewActiveAerial = true);
	
// protected:
// 	/** 공중에서 설정한 타이머 만큼 시간이 지나면 중력이 증가되어 캐릭터가 빨리 떨어지게 하는 함수입니다. */
// 	void UpdateGravityControlInTheAir();

protected:
	// /** UpdateGravityControlInTheAir 함수에서 중력을 설정할 때 딜레이를 주는 TimerHandle입니다. */
	// FTimerHandle GravityControlDelayTimerHandle;

	/** 기본 중력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityControl")
	float DefaultGravity;

	// /** 중력을 설정할 딜레이 시간입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityControl")
	// float GravityControlDelayTime;
	//
	// /** 증가할 중력의 크기입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityControl")
	// float IncreaseGravityScale;

	/** 공중에서 머무는(에어리얼) 상태인지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityControl")
	bool bActivateAerial;
#pragma endregion
};
