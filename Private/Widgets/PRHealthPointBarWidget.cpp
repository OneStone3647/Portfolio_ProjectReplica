// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRHealthPointBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/PRStatSystemComponent.h"

UPRHealthPointBarWidget::UPRHealthPointBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HealthPointBar = nullptr;
	HealthPointBarBuffer = nullptr;
	HealthPointBuffer = 1.0f;
	HealthPointBarBufferLerpSpeed = 0.02f;
	
	// StatSystem
	OwnerStatSystem = nullptr;
}

void UPRHealthPointBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthPointBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthPointBar")));
	HealthPointBarBuffer = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthPointBarBuffer")));

	// InitializeHealthPointBar();
}

void UPRHealthPointBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHealthPointBarBuffer();
}

// void UPRHealthPointBarWidget::InitializeHealthPointBar()
// {
// 	if(OwnerStatSystem != nullptr
// 		&& HealthPointBar != nullptr
// 		&& HealthPointBarBuffer != nullptr)
// 	{
// 		HealthPointBar->SetPercent(OwnerStatSystem->GetHealthPointRatio());
// 		HealthPointBarBuffer->SetPercent(OwnerStatSystem->GetHealthPointRatio());
// 	}
// }

void UPRHealthPointBarWidget::UpdateHealthPointBarBuffer()
{
	// HealthPointBuffer를 캐릭터의 최대 체력에 대한 현재 체력의 비율 값으로 보간하여 HealthPointBar가 대미지 받은 양만큼 줄어드는 효과를 나타냅니다.
	if(OwnerStatSystem != nullptr)
	{
		FPRCharacterStat CharacterStat = OwnerStatSystem->GetCharacterStat();
		HealthPointBuffer = FMath::Lerp(HealthPointBuffer, CharacterStat.HealthPoint / CharacterStat.MaxHealthPoint, HealthPointBarBufferLerpSpeed);
	}
}

float UPRHealthPointBarWidget::GetHealthPointBarPercent() const
{
	if(OwnerStatSystem != nullptr
		&& HealthPointBar != nullptr)
	{
		FPRCharacterStat CharacterStat = OwnerStatSystem->GetCharacterStat();
		return CharacterStat.HealthPoint / CharacterStat.MaxHealthPoint;
	}

	return 0.0f;
}

float UPRHealthPointBarWidget::GetHealthPointBarBufferPercent() const
{
	return HealthPointBuffer;
}

#pragma region StatSystem
void UPRHealthPointBarWidget::BindStatSystem(UPRStatSystemComponent* NewStatSystem)
{
	OwnerStatSystem = NewStatSystem;
}
#pragma endregion 
