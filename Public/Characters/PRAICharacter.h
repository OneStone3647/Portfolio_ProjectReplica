// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "PRAICharacter.generated.h"

class UPRTargetAimSystemComponent;
class UWidgetComponent;

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
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void TakeHit(AActor* DamageCauser) override;
	virtual void Dead() override;

#pragma region TargetAimSystem
public:
	/**
	 * Target으로 지정되었을 때 실행하는 함수입니다.
	 *
	 * @param bActivateLockedOn TargetAimSystem의 LockOnWidget의 활성화를 설정합니다.
	 */
	void LockedOnTarget(bool bActivateLockedOn);
	
protected:
	/** Targeting TraceChannel에 블록되는 Collision과 Target으로 지정되어 LockOn 되었을 때 활성화하는 위젯을 가진 SphereComponent 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetAimSystem")
	UPRTargetAimSystemComponent* TargetAimSystem;

public:
	/** TargetAimSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTargetAimSystemComponent* GetTargetAimSystem() const
	{
		return TargetAimSystem;
	}
	
public:
	/** TargetAim의 위치를 반환하는 함수입니다. */
	FVector GetTargetAimLocation() const;
#pragma endregion

#pragma region HealthPointBar
protected:
	/** HealthPointBar를 초기화하는 함수입니다. */
	void InitializeHealthPointBar();

	/** HealthPointBar가 PlayerCamera를 바라보도록 설정하는 함수입니다. */
	void HealthPointBarLookAtPlayerCamera();

private:
	/** 캐릭터의 HealthPoint를 나타내는 WidgetComponent입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HealthPointBar;

	/** 일정시간 피격 당하지 않으면 HealthPointBar를 숨기는 TimerHandle입니다. */
	FTimerHandle HealthPointBarHiddenTimerHandle;

	/** 피격당한 이후 다시 피격당하지 않아 HealthPointBar를 숨길 때까지 걸리는 시간입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	float HealthPointBarVisibleTimeSinceTakeHit;

public:
	/** HealthPointBart을 반환하는 함수입니다. */
	class UWidgetComponent* GetHealthPointBar() const;
#pragma endregion 
};
