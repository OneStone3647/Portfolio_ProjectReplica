// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/TimelineComponent.h"
#include "Skills/PRCooldownSkill.h"
#include "PRSkill_Kyle_WarpShift.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 캐릭터가 일정거리를 빠르게 접근하는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_Kyle_WarpShift : public UPRCooldownSkill
{
	GENERATED_BODY()

public:
	UPRSkill_Kyle_WarpShift();

public:
	virtual void Tick(float DeltaTime) override;

public:
	/** 스킬을 초기화하는 함수입니다. */
	virtual void InitializeSkill_Implementation() override;
	
	/**
	 * 스킬을 실행하는 함수입니다.
	 *
	 * @return 스킬이 정상적으로 실행됐을 경우 true를 반환합니다. 실행되지 않았을 경우 false를 반환합니다.
	 */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지 판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;

private:
	/** MoveLerpTimeline을 초기화하는 함수입니다. */
	void InitializeMoveLerpTimeline();

	/**
	 * 캐릭터를 선형보간 이동시키는 함수입니다.
	 * 
	 * @param Value Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void MoveLerp(float Value);

	/**	캐릭터를 선형보간 이동한후 실행하는 함수입니다. */
	UFUNCTION()
	void MoveLerpFinish();

	/** WarpShift할 위치를 계산하는 함수입니다. */
	FVector CalculateWarpShiftLocation() const;

	/** WarpShift를 마칠때 Owner의 MovementState를 예상하여 반환하는 함수입니다.*/
	EPRMovementState CalculateMovementStateWhenFinishWarpShift();

	/** 입력받은 인자 값에 따라 SkillOwner의 Collision을 활성화/비활성화를 설정하는 함수입니다. */
	void SetEnableCollision(bool bSetAble);
	
private:
	/** Curve가 사용할 Callback 함수입니다. */
	FOnTimelineFloat MoveLerpTimelineProgress;
	
	/** Timeline이 끝난 후 호출하는 Callback 함수입니다. */
	FOnTimelineEvent MoveLerpTimelineFinish;
	
	/** 캐릭터가 선형보간 이동할 때 사용하는 Timeline입니다. */
	FTimeline MoveLerpTimeline;

	/** WarpShift를 실행할 때 선형보간 이동에 사용하는 Curve입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MoveLerpFloatCurve;

	/** WarpShift 속도의 비율입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	float WarpShiftSpeedRatio;

	/** WarpShiftTarget이 없을 때 이동하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	float WarpDistance;

	/** WarpShift를 마칠 때 Owner가 공중에 있는지 지상에 있는지 탐지하기 위한 Owner의 발과 바닥 사이의 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	float DetectFloorDistance;

	/** Owner의 현재 위치입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	FVector CurrentOwnerLocation;

	/** WarpShift의 Target입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	AActor* WarpShiftTarget;

	/** WarpShift할 위치 입니다. WarpShiftTarget와 충돌을 일으키기 않게 보정한 위치입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WarpShift", meta = (AllowPrivateAccess = "true"))
	FVector WarpShiftLocation;

#pragma region Debug
private:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebugFlag;
#pragma endregion 

#pragma region Effect
private:
	/** RibbonEffect를 활성화하는 함수입니다. */
	void ActivateRibbonEffect();

	/** RibbonEffect를 비활성화하는 함수입니다. */
	void DeactivateRibbonEffect();
	
	/** RibbonEffect를 생성하고 반환하는 함수입니다. */
	UNiagaraComponent* SpawnRibbonEffect() const;
	
private:
	/** RibbonEffect를 바탕으로 생성한 NiagaraComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* RibbonEffectComponent;
	
	/** WarpShift할 때 사용하는 NiagaraSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* RibbonEffect;

	/** 생성한 RibbonEffect를 부착할 Mesh의 소켓이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	/** 생성하는 RibbonEffect의 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FVector LocationOffset;

	/** 생성하는 RibbonEffect의 회전 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FRotator RotationOffset;

	/** RibbonEffect의 색을 캐릭터의 SignatureEffectColor로 사용하지 않고 EffectColor로 덮어서 사용할지 여부를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	bool bOverrideEffectColor;
	
	/** RibbonEffect의 색입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true", EditCondition = "bOverrideEffectColor", EditConditionHides))
	FLinearColor EffectColor;

	/** RibbonEffect의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	float EffectSize;
#pragma endregion 

#pragma region PRAnimMontage
private:
	/** CastingWarpShiftPRAnimMontage의 ID입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	int32 CastingWarpShiftPRAnimMontageID;
	
	/** WarpShift 시전 PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage CastingWarpShiftPRAnimMontage;

	/** InGroundWarpShiftFinishPRAnimMontage의 ID입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	int32 InGroundWarpShiftFinishPRAnimMontageID;

	/** 지상에서 WarpShift를 마무리하는 PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage InGroundWarpShiftFinishPRAnimMontage;

	/** InAerialWarpShiftFinishPRAnimMontage의 ID입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	int32 InAerialWarpShiftFinishPRAnimMontageID;

	/** 공중에서 WarpShift를 마무리하는 PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage InAerialWarpShiftFinishPRAnimMontage;
#pragma endregion 
};
