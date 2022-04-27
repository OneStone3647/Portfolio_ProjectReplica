// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/DataTable.h"
#include "Struct_PRCharacterStat.generated.h"

/**
 * 캐릭터의 스탯(능력치)를 구성하는 변수를 가진 구조체입니다.
 * @Atomic		  이 구조체를 항상 하나의 단위로 직렬화되게 합니다. 이 구조체를 디테일 패널에서 사용할 수 있게 됩니다.
 *				  구조체 내부 변수들의 UPROPERTY 매크로에 EditAnywhere를 작성해야 합니다.
 * @BlueprintType 이 구조체를 블루프린트에서 사용할 수 있습니다.
 *				  구조체 내부 변수들의 UPROPERTY 매크로에 BlueprintReadWrite를 작성해야 합니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRCharacterStat : public FTableRowBase
{
    GENERATED_BODY()

public:
	FPRCharacterStat()
		: MaxHealthPoint(1000.0f), AttackPoint(10.0f)
	{}

	FPRCharacterStat(float NewMaxHealthPoint, float NewAttackPoint)
		: MaxHealthPoint(NewMaxHealthPoint), AttackPoint(NewAttackPoint)
	{}
	
public:
	/** 최대 체력을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharaterStat")
	float MaxHealthPoint;

	/**
	 * 캐릭터의 공격력을 나타내는 변수입니다.
	 * 무기의 대미지와는 별개로 적용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharaterStat")
	float AttackPoint;
};