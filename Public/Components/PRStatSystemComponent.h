// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRStatSystemComponent.generated.h"

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
	/** 인자로 받은 캐릭터 스탯으로 현재 캐릭터 스탯을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRStatSystem")
	void InitializeCharacterStat(const FPRCharacterStat& NewCharacterStat);
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
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
