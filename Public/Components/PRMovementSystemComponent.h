// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Enums/Enum_PRRotationMode.h"
#include "Enums/Enum_PRMovementState.h"
#include "Enums/Enum_PRGait.h"
#include "Components/TimelineComponent.h"
#include "PRMovementSystemComponent.generated.h"

/**
 * 캐릭터의 움직임을 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRMovementSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRMovementSystemComponent();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region MovementInfo
public:
	/** 캐릭터의 OnMovementModeChanged를 사용하여 EPRMovementState를 설정하는 함수입니다. */
	void OnCharacterMovementModeChanged(EMovementMode NewMovementMode);

	/**
	 * 입력받은 인자가 MovementState와 같은지 판별하는 함수입니다.
	 * 같은 경우 true, 다를 경우 false를 반환합니다. 
	 */
	bool IsEqualMovementState(EPRMovementState NewPRMovementState) const;

	/**
	 * 입력받은 인자가 Gait와 같은지 판별하는 함수입니다.
	 * 같을 경우 true, 다를 경우 false를 반환합니다.
	 */
	bool IsEqaulGait(EPRGait NewPRGait) const;

	/** 캐릭터가 움직일 수 있는지 판별하는 함수입니다. */
	bool IsCanMove() const;
	
	/** 캐릭터가 움직이고 있는지 판별하는 함수입니다. */
	bool IsMoving() const;

	/** 캐릭터가 가속상태인지 판별하는 함수입니다. */
	bool IsAccelerating() const;

protected:
	/** MovementInfo에 해당하는 변수들을 최신화하는 함수입니다. */
	void UpdateMovementInfo();
	
	/**
	 * 입력받은 걷는 모양에 해당하는 캐릭터의 MaxWalkSpeed를 설정하는 함수입니다.
	 * @NewPRGait: 설정할 캐릭터의 걷는 모양입니다.
	 */
	void SetCharacterMaxWalkSpeed(EPRGait NewPRGait);

	/**
	 * DecreaseMaxWalkSpeedTimeline을 초기화하는 함수입니다.
	 * Curve가 사용하는 Callback 함수에 사용할 함수를 바인드합니다.
	 * Timeline에 Curve를 추가합니다.
	 */
	void InitializeMaxWalkSpeedTimeline();
	
	/**
	 * 캐릭터의 MaxWalkSpeed를 감소하는 함수입니다.
	 * @Value: Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void DecreaseCharacterMaxWalkSpeed(float Value);

	/**
	 * DecreaseMaxWalkSpeedTimeline에서 사용할 Curve를 설정하는 함수입니다.
	 * AllowedGait와 PreviousAllowedGait의 값에 따라 Curve를 설정합니다.
	 */
	void SetDecreaseMaxWalkSpeedCurve();
	
protected:
	/** 캐릭터의 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRMovementState MovementState;

	/** 캐릭터의 이전 움직임 상태입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRMovementState PreviousMovementState;
	
	/** 캐릭터의 회전 모드입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRRotationMode RotationMode;

	/** 캐릭터의 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRGait Gait;

	/** 캐릭터의 허용된 걷는 모양입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRGait AllowedGait;

	/** 캐릭터의 이전 허용된 걷는 모양입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	EPRGait PreviousAllowedGait;

	/** 캐릭터가 움직일 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	bool bCanMove;
	
	/** 캐릭터가 움직이고 있는지 나타내는 변수입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	bool bIsMoving;

	/** 캐릭터가 가속 상태인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	bool bIsAccelerating;

	/** 멈췄을 때의 속도를 SpeedWhenStopping 변수에 저장했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	bool bIsSetSpeedWhenStopping;

	/** 멈췄을 때의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float SpeedWhenStopping;

	/** 게임패드(아날로그 스틱)으로 이동할 때 최소한의 기울기에서 걸을 때 설정한 속도만큼 가속합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float MinAnalogWalkSpeed;
	
	/** 캐릭터의 속도입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float Speed;

	/** 캐릭터의 걷기 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float WalkSpeed;
	
	/** 캐릭터의 달리기 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float RunSpeed;

	/** 캐릭터의 전력질주 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	float SprintSpeed;

	/** 캐릭터의 MaxWalkSpeed를 감소할 때 사용하는 Timeline입니다. */
	FTimeline DecreaseMaxWalkSpeedTimeline;

	/** RunSpeed에서 WalkSpeed로 값이 변하는 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	UCurveFloat* DecreaseRunToWalkSpeedCurveFloat;

	/** SprintSpeed에서 RunSpeed로 값이 변하는 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	UCurveFloat* DecreaseSprintToRunSpeedCurveFloat;

	/** SprintSpeed에서 WalkSpeed로 값이 변하는 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMovementSystem|MovementInfo")
	UCurveFloat* DecreaseSprintToWalkSpeedCurveFloat;

public:
	/** MovementState를 반환하는 함수입니다. */
	EPRMovementState GetMovementState() const;
	
	/**
	 * 입력받은 인자로 MovementState를 설정하는 함수입니다.
	 * MovementState를 설정하기 전 PreviousMovementState 값을 저장합니다.
	 */
	void SetMovementState(EPRMovementState NewPRMovementState);

	/** PreviousMovementState를 반환하는 함수입니다. */
	EPRMovementState GetPreviousMovementState() const;
	
	/**
	 * 입력받은 인자로 캐릭터의 회전 모드를 설정합니다.
	 * @NewRotationMode: 설정할 캐릭터의 회전 모드입니다.
	 */
	void SetRotationMode(EPRRotationMode NewRotationMode);

	/** Gait를 반환하는 함수입니다. */
	EPRGait GetGait() const;
	
	/**
	 * 입력받은 속도에 따라 Gait를 설정하는 함수입니다.
	 * @NewSpeed: 캐릭터의 속도입니다.
	 */
	void SetGait(float NewSpeed);

	/** AllowedGait를 반환하는 함수입니다. */
	EPRGait GetAllowedGait() const;

	/**
	 * 입력받은 인자로 AllowedGait를 설정하는 함수입니다.
	 * @NewPRGait: 설정할 캐릭터의 허용된 걷는 모양입니다.
	 */
	void SetAllowedGait(EPRGait NewPRGait);

	/** SpeedWhenStopping을 반환하는 함수입니다. */
	float GetSpeedWhenStopping() const;
	
	/** SpeedWhenStopping을 설정하는 함수입니다. */
	void SetSpeedWhenStopping();
	
	/** MinAnalogWalkSpeed를 반환하는 함수입니다. */
	float GetMinAnalogWalkSpeed() const;

	/** Speed를 반환하는 함수입니다. */
	float GetSpeed() const;

	/** WalkSpeed를 반환하는 함수입니다. */
	float GetWalkSpeed() const;
	
	/** RunSpeed를 반환하는 함수입니다. */
	float GetRunSpeed() const;

	/** SprintSpeed를 반환하는 함수입니다. */
	float GetSprintSpeed() const;
#pragma endregion

#pragma region GravityControl
protected:
	/** 공중에서 설정한 타이머 만큼읨 시간이 지나면 중력이 증가되어 캐릭터가 빨리 떨어지게 하는 함수입니다. */
	void UpdateGravityControlInTheAir();

public:
	/**
	 * 공중에서 머무는(에어리얼) 상태를 실행하는 함수입니다.
	 * @bFlag: true일 경우 이동을 중지(속도를 0으로 설정, 가속이 있는 구성요소의 경우 가속을 0으로 설정)하고
	 *			캐릭터의 GravityScale이 0.0f으로 설정합니다.
	 *			false일 경우 캐릭터의 GravityScale을 1.0f으로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMovementSystem|GravityControl")
	void ExecuteAerial(bool bFlag = false);

	/** 공중에서 머무는(에어리얼) 상태였는지를 반환하는 함수입니다. */
	bool WasAerial() const;
	
protected:
	/**
	 * UpdateGravityControlInTheAir 함수에서 사용할 TimerHandle입니다.
	 * 중력을 설정할 때 딜레이를 주기 위해 사용합니다.
	 */
	FTimerHandle GravityControlDelayTimerhandle;

	/** 중력설정의 딜레이 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|GravityControl")
	float GravityControlDelayTime;

	/** 증가한 중력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|GravityControl", Meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float IncreaseGravityScale;

	/** 공중에서 머무는(에어리얼) 상태인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|GravityControl")
	bool bIsAerial;

	/** 공중에서 머무는(에어리얼) 상태였는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRMovementSystem|GravityControl")
	bool bWasAerial;

public:
	/** 입력받은 인자로 bWasAerial 변수를 설정하는 함수입니다. */
	void SetWasAerial(bool bFlag);
#pragma endregion 
};
