// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 프로젝트에서 사용하는 구조체를 정의한 파일입니다.
 * 
 * USTRUCT 매크로의 키워드
 * @Atomic: 이 구조체를 항상 하나의 단위로 직렬화되게 합니다. 이 구조체를 디테일 패널에서 사용할 수 있게 됩니다.
 *			구조체 내부 변수들의 UPROPERTY 매크로에 EditAnywhere를 작성해야합니다.
 * @BlueprintType: 이 구조체를 블루프린트에서 사용할 수 있습니다.
 *				   구조체 내부 변수들의 UPROPERTY 매크로에 BlueprintReadWrite를 작성해야합니다.
 */

#include "CoreMinimal.h"
#include "PRCommonEnum.h"
#include "Engine/DataTable.h"
#include "PRCommonStruct.generated.h"

class APRAICharacter;

#pragma region PRAnimMontage
/**
 * 재생할 AnimMontage와 재생 옵션을 관리하는 변수를 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAnimMontage : public FTableRowBase
{
    GENERATED_BODY()

public:
	FPRAnimMontage()
		: AnimMontageID(0)
		, AnimMontageName(FString())
		, AnimMontage(nullptr)
		, PlayRate(1.0f)
		, MoveDistance(0.0f)
		, MoveSpeedRatio(1.0f)
		, MoveLerpFloatCurve(nullptr)

	{}

	FPRAnimMontage(int32 NewAnimMontageID, FString NewAnimMontageName, UAnimMontage* NewAnimMontage, float NewPlayRate, float NewMoveDistance, float NewMoveSpeedRatio, UCurveFloat* NewMoveLerpFloatCurve)
		: AnimMontageID(NewAnimMontageID)
		, AnimMontageName(NewAnimMontageName)
		, AnimMontage(NewAnimMontage)
		, PlayRate(NewPlayRate)
		, MoveDistance(NewMoveDistance)
		, MoveSpeedRatio(NewMoveSpeedRatio)
		, MoveLerpFloatCurve(NewMoveLerpFloatCurve)
	{}

public:
	/** AnimMontage의 ID입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	int32 AnimMontageID;

	/** AnimMontage의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	FString AnimMontageName;
	
	/** 재생할 AnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	UAnimMontage* AnimMontage;

	/** AnimMontage의 재생속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	float PlayRate;

	/** AnimMontage를 재생할 때 이동하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	float MoveDistance;

	/** AnimMontage를 재생할 때 이동하는 속도의 배율입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	float MoveSpeedRatio;

	/** AnimMontage를 재생할 때 선형보간 이동에 사용하는 Curve입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	UCurveFloat* MoveLerpFloatCurve;

public:
	/**
	 * 매개변수로 받은 PRAnimMontage와 같은지 판별하는 ==연산자 오버로딩입니다.
	 * 
	 * @param NewPRAnimMontage 같은지 판별할 PRAnimMontage입니다.
	 * @return 매개변수로 받은 PRAnimMontage와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRAnimMontage& NewPRAnimMontage) const
	{
		return this->AnimMontageID == NewPRAnimMontage.AnimMontageID
				&& this->AnimMontageName == NewPRAnimMontage.AnimMontageName
				&& this->AnimMontage == NewPRAnimMontage.AnimMontage
				&& this->PlayRate == NewPRAnimMontage.PlayRate
				&& this->MoveDistance == NewPRAnimMontage.MoveDistance
				&& this->MoveSpeedRatio == NewPRAnimMontage.MoveSpeedRatio
				&& this->MoveLerpFloatCurve == NewPRAnimMontage.MoveLerpFloatCurve;
	}

	/**
	 * 매개변수로 받은 PRAnimMontage와 다른지 판별하는 !=연산자 오버로딩입니다.
	 * 
	 * @param NewPRAnimMontage 다른지 판별할 PRAnimMontage입니다.
	 * @return 매개변수로 받은 PRAnimMontage와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRAnimMontage& NewPRAnimMontage) const
	{
		return this->AnimMontageID != NewPRAnimMontage.AnimMontageID
				|| this->AnimMontageName != NewPRAnimMontage.AnimMontageName
				|| this->AnimMontage != NewPRAnimMontage.AnimMontage
				|| this->PlayRate != NewPRAnimMontage.PlayRate
				|| this->MoveDistance != NewPRAnimMontage.MoveDistance
				|| this->MoveSpeedRatio != NewPRAnimMontage.MoveSpeedRatio
				|| this->MoveLerpFloatCurve != NewPRAnimMontage.MoveLerpFloatCurve;
	}
};

/**
 * 콤보로 사용하는 AnimMontage와 설정 값을 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRComboAnimMontage : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRComboAnimMontage()
		: PRAnimMontages(TArray<FPRAnimMontage>())
		, PlayIndex(0)
	{}

	FPRComboAnimMontage(TArray<FPRAnimMontage> NewPRAnimMontages, int32 NewPlayIndex = 0)
		: PRAnimMontages(NewPRAnimMontages)
		, PlayIndex(NewPlayIndex)
	{}
	
public:
	/** 콤보로 사용하는 PRAnimMontage를 보관한 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRComboAnimMontage")
	TArray<FPRAnimMontage> PRAnimMontages;

	/** 재생하는 PRAnimMontage의 Index 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRComboAnimMontage")
	int32 PlayIndex;

public:
	/** PlayIndex를 초기화하는 함수입니다. */
	void InitializePlayIndex()
	{
		PlayIndex = 0;
	}
	
	/**
	 * PlayIndex를 1 증가 시키는 함수입니다.
	 * PlayIndex가 PRAnimMontages의 크기와 같을 경우 0으로 설정합니다.
	 */
	void IncreasePlayIndex()
	{
		if(PlayIndex + 1 == PRAnimMontages.Num()
			|| PRAnimMontages.Num() == 0)
		{
			PlayIndex = 0;
		}
		else
		{
			PlayIndex++;
		}
	}

	/** 인자 값에 해당하는 Index 값을 가진 PRAnimMontage가 존재하는지 판별하는 함수입니다. */
	bool IsValidPlayIndex(int32 NewPlayIndex) const
	{
		return PRAnimMontages.IsValidIndex(NewPlayIndex);
	}

	/** 현재 재생하는 PRAnimMontage를 반환하는 함수입니다. */
	FPRAnimMontage GetPlayPRAnimMontage() const
	{
		if(IsValidPlayIndex(PlayIndex) == true)
		{
			return PRAnimMontages[PlayIndex];
		}

		return FPRAnimMontage();
	}

	TArray<FPRAnimMontage> GetPRAnimMontages() const
	{
		return PRAnimMontages;
	}

	void SetPRAnimMontages(const TArray<FPRAnimMontage>& NewPRAnimMontages)
	{
		PRAnimMontages = NewPRAnimMontages;
	}

	/** PlayIndex를 반환하는 함수입니다. */
	int32 GetPlayIndex() const
	{
		return PlayIndex;
	}

	/** 입력받은 인자로 PlayIndex를 설정하는 함수입니다. */
	void SetPlayIndex(int32 NewPlayIndex)
	{
		PlayIndex = NewPlayIndex;
	}

public:
	FPRComboAnimMontage& operator=(const FPRComboAnimMontage& NewPRComboAnimMontage)
	{
		this->PRAnimMontages = NewPRComboAnimMontage.PRAnimMontages;
		this->PlayIndex = NewPRComboAnimMontage.PlayIndex;

		return *this;
	}

	/**
	 * 매개변수로 받은 PRAnimMontage와 같은지 판별하는 ==연산자 오버로딩입니다.
	 * 
	 * @param NewPRAnimMontage 같은지 판별할 PRAnimMontage입니다.
	 * @return 매개변수로 받은 PRAnimMontage와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRComboAnimMontage& NewPRComboAnimMontage) const
	{
		return this->PRAnimMontages == NewPRComboAnimMontage.PRAnimMontages
				&& this->PlayIndex == NewPRComboAnimMontage.PlayIndex;
	}

	/**
	 * 매개변수로 받은 PRAnimMontage와 다른지 판별하는 !=연산자 오버로딩입니다.
	 * 
	 * @param NewPRAnimMontage 다른지 판별할 PRAnimMontage입니다.
	 * @return 매개변수로 받은 PRAnimMontage와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRComboAnimMontage& NewPRComboAnimMontage) const
	{
		return this->PRAnimMontages != NewPRComboAnimMontage.PRAnimMontages
				|| this->PlayIndex != NewPRComboAnimMontage.PlayIndex;
	}
};

/**
 * 공격에 사용하는 AnimMontage와 설정 값을 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAttackAnimMontage : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRAttackAnimMontage()
		: AttackPRAnimMontages(TArray<FPRAnimMontage>())
		, PlayPRAnimMontageIndex(0)
	{}

	FPRAttackAnimMontage(TArray<FPRAnimMontage> NewAttackPRAnimMontages, int32 NewPlayPRAnimMontageIndex = 0)
		: AttackPRAnimMontages(NewAttackPRAnimMontages)
		, PlayPRAnimMontageIndex(NewPlayPRAnimMontageIndex)
	{}
	
public:
	/** 공격 PRAnimMontage를 보관한 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAttackAnimMontage")
	TArray<FPRAnimMontage> AttackPRAnimMontages;

	/** 재생하는 PRAnimMontage의 Index 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAttackAnimMontage")
	int32 PlayPRAnimMontageIndex;

public:
	/** PlayPRAnimMontageIndex를 초기화하는 함수입니다. */
	void InitializePlayPRAnimMontageIndex()
	{
		PlayPRAnimMontageIndex = 0;
	}
	
	/**
	 * PlayPRAnimMontageIndex를 1 증가 시키는 함수입니다.
	 * PlayPRAnimMontageIndex가 PRAnimMontages의 크기와 같을 경우 0으로 설정합니다.
	 */
	void IncreasePlayPRAnimMontageIndex()
	{
		if(PlayPRAnimMontageIndex + 1 == AttackPRAnimMontages.Num()
			|| AttackPRAnimMontages.Num() == 0)
		{
			PlayPRAnimMontageIndex = 0;
		}
		else
		{
			PlayPRAnimMontageIndex++;
		}
	}

	/** PlayPRAnimMontageIndex에 해당하는 Index 값을 가진 AttackPRAnimMontage가 존재하는지 판별하는 함수입니다. */
	bool IsValidPlayPRAnimMontageIndex() const
	{
		return AttackPRAnimMontages.IsValidIndex(PlayPRAnimMontageIndex);
	}

	/** 재생하는 PRAnimMontage를 반환하는 함수입니다. */
	FPRAnimMontage GetPlayAttackPRAnimMontage() const
	{
		if(IsValidPlayPRAnimMontageIndex() == true)
		{
			return AttackPRAnimMontages[PlayPRAnimMontageIndex];
		}

		return FPRAnimMontage();
	}

	TArray<FPRAnimMontage> GetAttackPRAnimMontages() const
	{
		return AttackPRAnimMontages;
	}

	/** PlayPRAnimMontageIndex를 반환하는 함수입니다. */
	int32 GetPlayPRAnimMontageIndex() const
	{
		return PlayPRAnimMontageIndex;
	}

	/** 입력받은 인자로 PlayPRAnimMontageIndex를 설정하는 함수입니다. */
	void SetPlayPRAnimMontageIndex(int32 NewPlayPRAnimMontageIndex)
	{
		PlayPRAnimMontageIndex = NewPlayPRAnimMontageIndex;
	}
};
#pragma endregion

/**
 * 상호작용에 대한 정보를 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRInteractInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRInteractInfo()
		: InteractTarget()
		, Description()
		, Icon()
	{}

	FPRInteractInfo(AActor* NewInteractTarget, FString NewDescription, UTexture2D* NewIcon)
		: InteractTarget(NewInteractTarget)
		, Description(NewDescription)
		, Icon(NewIcon)
	{}
	

public:
	/** 상호작용하는 대상입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInteractInfo")
	AActor* InteractTarget;
	
	/** 상호작용 UI에 사용하는 설명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInteractInfo")
	FString Description;

	/** 상호작용 UI에 사용하는 아이콘입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInteractInfo")
	UTexture2D* Icon;
};

/**
 * 입력키와 입력키에 해당하는 아이콘을 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRInputKey : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRInputKey()
		: Key()
		, Icon()
	{}

	FPRInputKey(FKey NewKey, UTexture2D* NewIcon)
		: Key(NewKey)
		, Icon(NewIcon)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRKey")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRKey")
	UTexture2D* Icon;
};

/**
 * 설정할 Able(가능 상태)와 설정 값을 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAbleStateStruct
{
	GENERATED_BODY()

public:
	/** 설정할 Able(가능 상태)입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAbleStatestruct")
	EPRAbleState AbleState;

	/** AbleState의 설정 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAbleStatestruct")
	bool bAble;

	/** AbleState의 설정하는 딜레이 시간입니다. 설정할 Able이 Dodgeable일 경우 활성화합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAbleStatestruct", meta = (EditCondition = "AbleState == EPRAbleState::AbleState_Dodgeable && bAble == true", EditConditionHides))
	float DelayTime;
};

/**
 * 캐릭터의 액션 상태를 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActionState
{
	GENERATED_BODY()

public:
	FPRActionState()
	: Action()
	, bActivate()
	, bAble()
	{}

	FPRActionState(EPRAction NewAction, bool bNewActivate, bool bNewAble)
		: Action(NewAction)
		, bActivate(bNewActivate)
		, bAble(bNewAble)
	{}

public:;
	/** 캐릭터의 액션입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRActionState")
	EPRAction Action;

	/** 액션의 실행 상태입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRActionState")
	bool bActivate;

	/** 액션을 실행할 수 있는 상태입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRActionState")
	bool bAble;
	
public:
	/**
	 * 인자로 받은 PRActionState와 같은지 판별하는 ==연산자 오버로딩입니다.
	 * 
	 * @param NewPRActionState 비교하는 PRActionState와 같은지 판별할 PRActionState입니다.
	 * @return 인자로 받은 PRActionState와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRActionState& NewPRActionState) const
	{
		return this->Action == NewPRActionState.Action
				&& this->bActivate == NewPRActionState.bActivate
				&& this->bAble == NewPRActionState.bAble;
	}

	/**
	 * 인자로 받은 PRActionState와 다른지 판별하는 !=연산자 오버로딩입니다.
	 * 
	 * @param NewPRActionState 비교하는 PRActionState와 같은지 판별할 PRActionState입니다.
	 * @return 인자로 받은 PRActionState와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRActionState& NewPRActionState) const
	{
		return this->Action != NewPRActionState.Action
				|| this->bActivate != NewPRActionState.bActivate
				|| this->bAble != NewPRActionState.bAble;
	}
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float MaxHealthPoint;

	/** 현재 체력입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRCharacterStat", Transient)
	float HealthPoint;
	
	/**
	 * 캐릭터의 공격력입니다.
	 * 무기의 대미지와는 별개로 적용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float AttackPoint;

	/** 캐릭터의 방어력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float DefencePoint;

	/** 캐릭터의 치명타 확률입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float CriticalRate;

	/** 캐릭터의 치명타 피해입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCharacterStat")
	float CriticalDamage;

public:
	/**
	 * 인자로 받은 PRCharacterStat와 같은지 판별하는 == 연산자 오버로딩입니다.
	 * 
	 * @param NewPRCharacterStat 비교하는 PRCharacterStat와 같은지 판별할 PRCharacterStat입니다.
	 * @return 인자로 받은 PRAICharacterInfo와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRCharacterStat& NewPRCharacterStat) const
	{
		return this->MaxHealthPoint == NewPRCharacterStat.MaxHealthPoint
				&& this->AttackPoint == NewPRCharacterStat.AttackPoint
				&& this->DefencePoint == NewPRCharacterStat.DefencePoint
				&& this->CriticalRate == NewPRCharacterStat.CriticalRate
				&& this->CriticalDamage == NewPRCharacterStat.CriticalDamage;
	}

	/**
	 * 인자로 받은 PRCharacterStat와 다른지 판별하는 != 연산자 오버로딩입니다.
	 * 
	 * @param NewPRCharacterStat 비교하는 PRCharacterStat와 같은지 판별할 PRCharacterStat입니다.
	 * @return 인자로 받은 PRCharacterStat와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRCharacterStat& NewPRCharacterStat) const
	{
		return this->MaxHealthPoint != NewPRCharacterStat.MaxHealthPoint
				|| this->AttackPoint != NewPRCharacterStat.AttackPoint
				|| this->DefencePoint != NewPRCharacterStat.DefencePoint
				|| this->CriticalRate != NewPRCharacterStat.CriticalRate
				|| this->CriticalDamage != NewPRCharacterStat.CriticalDamage;
	}
};

