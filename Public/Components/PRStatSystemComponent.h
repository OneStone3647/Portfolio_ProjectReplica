// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRStatSystemComponent.generated.h"

/** 캐릭터의 스탯(능력치)를 구성하는 변수를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRCharacterStat()
		: MaxHealthPoint(500.0f)
		, HealthPoint(500.0f)
		, AttackPoint(10.0f)
		, DefencePoint(10.0f)
		, CriticalRate(10.0f)
		, CriticalDamage(50.0f)
	{}

	FPRCharacterStat(float NewMaxHealthPoint, float NewHealthPoint, float NewAttackPoint, float NewDefencePoint, float NewCriticalRate,
						float NewCriticalDamage)
		: MaxHealthPoint(NewMaxHealthPoint)
		, HealthPoint(NewHealthPoint)
		, AttackPoint(NewAttackPoint)
		, DefencePoint(NewDefencePoint)
		, CriticalRate(NewCriticalRate)
		, CriticalDamage(NewCriticalDamage)
	{}
	

public:
	/** 최대 체력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRCharacterStat")
	float MaxHealthPoint;

	/** 현재 체력입니다. */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float HealthPoint;
	
	/**
	 * 캐릭터의 공격력입니다.
	 * 무기의 대미지와는 별개로 적용됩니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRCharacterStat")
	float AttackPoint;

	/** 캐릭터의 방어력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRCharacterStat")
	float DefencePoint;

	/** 캐릭터의 치명타 확률입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRCharacterStat")
	float CriticalRate;

	/** 캐릭터의 치명타 피해입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRCharacterStat")
	float CriticalDamage;
};

DECLARE_MULTICAST_DELEGATE(FOnHealthPointIsChangedDelegate)
DECLARE_MULTICAST_DELEGATE(FOnHealthPointIsZeroDelegate)

/**
 * 캐릭터의 능력치를 관리하는 ActorComponent입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRStatSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStatSystemComponent();

protected:
	virtual void InitializeComponent() override;

public:
	/** 캐릭터가 받은 대미지를 적용하는 함수입니다. */
	void TakeDamage(float NewDamage);

	// /** 캐릭터의 최대 체력에 대한 현재 체력의 비율을 반환하는 함수입니다. */
	// float GetHealthPointRatio() const;

	/**
	 * 입력받은 인자로 캐릭터의 현재 체력을 설정하고 체력에 따라 Delegate를 실행하는 함수입니다.
	 *
	 * @param NewHealthPoint 캐릭터의 현재 체력으로 설정할 값입니다.
	 */
	void SetHealthPoint(float NewHealthPoint);
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	FPRCharacterStat CharacterStat;
	
	/** 캐릭터의 성별입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EPRGender Gender;
	
public:
	/** CharacterStat을 반환하는 함수입니다. */
	FPRCharacterStat GetCharacterStat() const;
	
	/** Gender를 반환하는 함수입니다. */
	EPRGender GetGender() const;

public:
	/** 캐릭터의 현재 체력이 변했을 경우 실행하는 Delegate입니다. */
	FOnHealthPointIsChangedDelegate OnHealthPointIsChangedDelegate;

	/** 캐릭터의 현재 체력이 0일 경우 실행하는 Delegate입니다. */
	FOnHealthPointIsZeroDelegate OnHealthPointIsZeroDelegate;
};
