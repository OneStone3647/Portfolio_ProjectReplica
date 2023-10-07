// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PRBaseActorComponent.h"
#include "PRTimeStopSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateTimeStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivateTimeStop);

/**
 * 플레이어 캐릭터를 제외한 일정범위 안에 존재하는 Actor들을 일시정지하는 TimeStop을 실행하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRTimeStopSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRTimeStopSystemComponent();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	 * TimeStop을 실행하는 함수입니다.
	 * TimeStopRange 범위 내에 존재하는 오브젝트들의 CustomTimeDilation을 TimeStopDilation으로 설정합니다.
	 * @param NewTimeStopDuration 적용할 TimeStop의 시간입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void ActivateTimeStop(float NewTimeStopDuration = 4.0f);

	/** TimeStop을 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void DeactivateTimeStop();

	/** TimeStop을 실행하고 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	bool IsActivateTimeStop() const;

	/** TimeStop을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void UpdateTimeStop(float DeltaTime);

protected:
	/** Owner의 CustomTimeDilation을 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void SetOwnerCustomTimeDilation(float NewCustomTimeDilation);

	/**
	 * 입력받은 값으로 PostProcessSettings의 MotionBlurAmount 값을 설정하는 함수입니다.
	 *
	 * @param PostProcessSettings MotionBlurAmount 값을 설정할 PostProcessSettings입니다.
	 * @param NewMotionBlurAmount 설정할 MotionBlurAmount 값입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void ChangeMotionBlurAmount(UPARAM(ref) FPostProcessSettings& PostProcessSettings, float NewMotionBlurAmount);

protected:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeStop")
	bool bDebug;
		
	/** TimeStop의 실행을 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeStop")
	bool bActivate;
	
	/** 범위를 지정하여 TimeStop을 실행할지 나타내는 변수입니다. false일 경우 월드 전체를 범위로 TimeStop울 실행합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeStop")
	bool bActivateTimeStopRange;
	
	/** TimeStop이 적용된 Actor들입니다.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeStop")
	TArray<AActor*> TimeStopTarget;
	
	/** TimeStop의 지속시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeStop")
	float TimeStopDuration;

	/** TimeStop의 남은 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeStop")
	float TimeStopRemaining;

	/** TimeStop의 경과시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeStop")
	float TimeStopElapsed;
	
	/** TimeStop 실행시 적용할 시간 흐름 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeStop")
	float TimeStopDilation;

	/** Owner의 시간 흐름 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeStop")
	float OwnerTimeDilation;

	/** TimeStop에서 사용하는 TimerHandle입니다. */
	FTimerHandle TimeStopTimerHandle;
	
	/** TimeStop을 활성화하는 범위입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeStop")
	FVector TimeStopRange;

	/** TimeStop 실행시 화면의 블러 현상을 조절하기 위한 MotionBlur의 강도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeStop")
	float BaseMotionBlurIntensity;

public:
	/** TimeStopDuration을 반환하는 함수입니다. */
	float GetTimeStopDuration() const;

public:
	/** TimeStop이 활성화일 때 실행하는 Delegate입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "TimeStop")
	FOnActivateTimeStop OnActivateTimeStop;
	
	/** TimeStop이 비활성화일 때 실행하는 Delegate입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "TimeStop")
	FOnDeactivateTimeStop OnDeactivateTimeStop;
};
