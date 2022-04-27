// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRHealthPointBarWidget.generated.h"

/**
 * 캐릭터의 HealthPoint를 나타내는 UserWidget 클래스입니다.
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

#pragma region StatSystem
public:
	/** 입력받은 인자로 StatSystem을 바인딩하는 함수입니다. */
	void BindStatSystem(class UPRStatSystemComponent* NewStatSystem);

private:
	/** 이 위젯을 사용하는 캐릭터의 StatSystem입니다. */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UPRStatSystemComponent* OwnerStatSystem;
#pragma endregion

#pragma region HealthPointBar
private:
	/** HealthPointBar를 최신화하는 함수입니다. */
	void UpdateHealthPointBar();

	/** HealthPointBarBuffer를 최신화하는 함수입니다. */
	void UpdateHealthPointBarBuffer();

private:
	/** HealthPointBar입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", BindWidget))
	class UProgressBar* HealthPointBar;

	/** HealthPoint가 변화되어 나타나는 HealthPointBar의 Buffer입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", BindWidget))
	class UProgressBar* HealthPointBarBuffer;

	/** HealthPointBarBuffer의 변화 값의 속도입니다. */ 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", BindWidget))
	float HealthPointBarBufferSpeed;
#pragma endregion 
};
