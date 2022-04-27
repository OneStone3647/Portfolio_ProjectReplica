// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Structs/Struct_PRCharacterStat.h"
#include "PRStatSystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHealthPointIsChangedDelegate)
DECLARE_MULTICAST_DELEGATE(FOnHealthPointIsZeroDelegate)

/**
 * 캐릭터의 스탯(능력치)를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRStatSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStatSystemComponent();

protected:
	/**
	 * 액터의 PostInitializeComponents에 대응하는 컴포넌트의 함수입니다.
	 * 액터의 PostInitializeComponents 함수가 호출되기 바로 전에 호출됩니다.
	 * 이 함수가 호출되려면 생성자에서 bWantsInitializeComponent의 값을 true로 설정해야 합니다.
	 */
	virtual void InitializeComponent() override;

public:
	/**
	 * 캐릭터가 받은 대미지를 적용하는 함수입니다.
	 * @param Damage 적용할 대미지 입니다.
	 */
	void TakeDamage(float Damage);

	/** 캐릭터의 현재 체력의 비율을 반환하는 함수입니다. */
	float GetHealthPointRatio() const;

private:
	/** 캐릭터의 스탯을 나타내는 구조체입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRStatSystem|CharacterStat", meta = (AllowPrivateAccess = "true"))
	FPRCharacterStat CharacterStat;

	/** 캐릭터의 현재 체력입니다. */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category = "PRStatSystem|CharacterStat", meta = (AllowPrivateAccess = "true"))
	float HealthPoint;
	
public:
	/** 캐릭터의 현재 체력이 변했을 경우 실행하는 Delegate입니다. */
	FOnHealthPointIsChangedDelegate OnHealthPointIsChanged;

	/** 캐릭터읜 현재 체력이 0일 경우 실행하는 Delegate입니다. */
	FOnHealthPointIsZeroDelegate OnHealthPointIsZero;
	
public:
	/** CharacterStat을 반환하는 함수입니다. */
	FPRCharacterStat GetCharacterStat() const;

	/**
	 * 입력받은 인자로 캐릭터의 체력을 설정하고 체력에 따라 Delegate를 실행하는 함수입니다.
	 * @param NewHealthPoint 캐릭터의 체력으로 설정한 체력 값입니다. 
	 */
	void SetHealthPoint(float NewHealthPoint);
};
