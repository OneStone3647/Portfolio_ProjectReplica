// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Weapons/PRMeleeWeapon.h"
#include "ANS_PRActivateAttackTrace.generated.h"

class APRBaseCharacter;
class APRMeleeWeapon;

/**
 * 실행할 AttackTrace의 종류를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRTraceType : uint8
{
	TraceType_LineTrace					UMETA(DisplayName = "LineTrace"),
	TraceType_SphereTraceByOwner		UMETA(DisplayName = "SphereTraceByOwner")
};

/**
 * 캐릭터가 발도한 무기의 AttackTrace를 실행하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRActivateAttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRActivateAttackTrace();

protected:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/**
	 * 인자로 받은 캐릭터가 장비한 무기를 반환하는 함수입니다.
	 *
	 * @param NewPROwner 장비한 무기를 가진 캐릭터입니다.
	 */
	APRMeleeWeapon* GetEquippedWeapon(class APRBaseCharacter* NewPROwner);
	
protected:
	/** AttackTrace를 실행할 무기의 위치를 나타내는 열거형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackTrace")
	EPRWeaponPosition WeaponPosition;

	/** 실행할 AttackTrace의 종류입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttactTrace")
	EPRTraceType TraceType;

	/** SphereTrace의 크기입니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttactTrace", meta = (EditCondition = "TraceType == EPRTraceType::TraceType_SphereTraceByOwner", EditConditionHides))
	float SphereTraceSize;

	/** AttackTrace의 디버그 실행을 결정하는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bActivateAttackTraceDebug;
};
