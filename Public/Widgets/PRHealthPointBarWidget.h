// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRHealthPointBarWidget.generated.h"

class UPRStatSystemComponent;
class UProgressBar;

/**
 * 캐릭터의 HealthPoint를 나타내는 UserWidget입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRHealthPointBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRHealthPointBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

// public:
// 	/** HealthPointBar를 초기화하는 함수입니다. */
// 	void InitializeHealthPointBar();

private:
	/** HealthPointBarBuffer를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "HealthPointBar")
	void UpdateHealthPointBarBuffer();

	/** HealthPointBar에 적용할 퍼센트 값을 계산하여 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "HealthPointBar")
	float GetHealthPointBarPercent() const;

	/** HealthPointBarBuffer에 적용할 퍼센트 값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "HealthPointBar")
	float GetHealthPointBarBufferPercent() const;
	
private:
	/** 이 위젯을 사용하는 캐릭터의 HealthPoint를 나타내는 ProgressBar입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true", BindWidget))
	UProgressBar* HealthPointBar;

	/** HealthPoint의 값이 변경되어 나타나는 HealthPointBar의 Buffer입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true", BindWidget))
	UProgressBar* HealthPointBarBuffer;

	/** HealthPointBuffer 값입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Transient, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	float HealthPointBuffer;
	
	/** HealthPointBarBuffer의 보간 속도입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	float HealthPointBarBufferLerpSpeed;
#pragma region StatSystem
public:
	/** 입력받은 인자로 StatSystem을 바인딩하는 함수입니다. */
	void BindStatSystem(class UPRStatSystemComponent* NewStatSystem);

private:
	/** 이 위젯을 사용하는 캐릭터의 StatSystem입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatSystem", meta = (AllowPrivateAccess = "true"))
	UPRStatSystemComponent* OwnerStatSystem;
#pragma endregion
};
