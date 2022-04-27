// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRTargetAimSystemComponent.h"
#include "PRAICharacter.generated.h"

/**
 * AI 캐릭터 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API APRAICharacter : public APRBaseCharacter
{
	GENERATED_BODY()

public:
	APRAICharacter();

protected:
	virtual void BeginPlay() override;

#pragma region TargetAimSystem
public:
	/**
	 * Target으로 지정되었을 때 실행하는 함수입니다.
	 * 입력된 인자에 따라 TargetAimSystem의 LockWidget을 활성화를 설정합니다.
	 * @param bFlag true일 경우 TargetAimSystem을 활성화합니다.
	 *				false일 경우 TargetAimSystem을 비활성화 합니다.
	 */
	void SelectedTarget(bool bFlag);
	
protected:
	/** Targeting TraceChannel에 블록되는 Collision과 Target으로 지정되어 LockOn 되었을 때 활성화하는 위젯을 가진 SphereComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetAimSystem")
	class UPRTargetAimSystemComponent* TargetAimSystem;

public:
	/** TargetAimSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTargetAimSystemComponent* GetTargetAimSystem() const
	{
		return TargetAimSystem;
	}

	/** TargetAimSystem의 위치를 반환하는 함수입니다. */
	FVector GetTargetAimLocation() const;
#pragma endregion

#pragma region HealthPointBarWidget
protected:
	/** HealthPointBarWidget를 초기화하는 함수입니다. */
	void InitHealthPointBarWidget();

protected:
	/** AI 캐릭터의 HealthPoint를 나타내는 WidgetComponent 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthPointWidget")
	class UWidgetComponent* HealthPointBarWidget;
#pragma endregion 
};
