// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/DataTable.h"
#include "Struct_PRAnimMontage.generated.h"

/**
 * 재생할 AnimMontage와 재생 옵션을 설정하는 변수를 가진 구조체입니다.
 * @Atomic: 이 구조체를 항상 하나의 단위로 직렬화되게 합니다. 이 구조체를 디테일 패널에서 사용할 수 있게 됩니다.
 *			구조체 내부 변수들의 UPROPERTY 매크로에 EditAnywhere를 작성해야 합니다.
 * @BlueprintType: 이 구조체를 블루프린트에서 사용할 수 있습니다.
 *				   구조체 내부 변수들의 UPROPERTY 매크로에 BlueprintReadWrite를 작성해야 합니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAnimMontage : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRAnimMontage()
		: AnimMontage(nullptr), PlayRate(1.0f), MoveDistance(0.0f), MoveSpeedRatio(1.0f), MoveLerpCurveFloat(nullptr)
	{}

	FPRAnimMontage(UAnimMontage* NewAnimMontage, float NewPlayRate, float NewMoveDistance, float NewMoveSpeedRatio, UCurveFloat* NewMoveLerpCurveFloat)
		: AnimMontage(NewAnimMontage), PlayRate(NewPlayRate), MoveDistance(NewMoveDistance), MoveSpeedRatio(NewMoveSpeedRatio), MoveLerpCurveFloat(NewMoveLerpCurveFloat)
	{}
	
public:
	/** 재생할 AnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	UAnimMontage* AnimMontage;

	/** AnimMontage의 재생속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	float PlayRate;

	/** AnimMontage를 재생할 때 이동하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	float MoveDistance;

	/** AnimMontage를 재생할 때 이동하는 속도의 배율입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	float MoveSpeedRatio;

	/** AnimMontage를 재생할 때 선형보간 이동에 사용하는 CurveFloat입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimMontage")
	UCurveFloat* MoveLerpCurveFloat;

public:
	/**
	 * 인자로 받은 PRAnimMontage와 같은지 비교하는 ==연산자 오버로딩입니다.
	 * @NewPRAnimMontage: 비교하는 PRAnimMontage입니다.
	 */
	FORCEINLINE bool operator==(const FPRAnimMontage& NewPRAnimMontage) const
	{
		return AnimMontage == NewPRAnimMontage.AnimMontage
				&& PlayRate == NewPRAnimMontage.PlayRate
				&& MoveDistance == NewPRAnimMontage.MoveDistance
				&& MoveSpeedRatio == NewPRAnimMontage.MoveSpeedRatio
				&& MoveLerpCurveFloat == NewPRAnimMontage.MoveLerpCurveFloat;
	}
};
