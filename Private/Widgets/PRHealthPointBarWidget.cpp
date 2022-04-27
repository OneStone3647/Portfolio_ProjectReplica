// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRHealthPointBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/PRStatSystemComponent.h"

UPRHealthPointBarWidget::UPRHealthPointBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthPointBar
	HealthPointBar = nullptr;
	HealthPointBarBuffer = nullptr;
	HealthPointBarBufferSpeed = 0.02f;
}

void UPRHealthPointBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthPointBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthPointBar")));
	HealthPointBarBuffer = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthPointBarBuffer")));

	UpdateHealthPointBar();
}

void UPRHealthPointBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHealthPointBarBuffer();
}

void UPRHealthPointBarWidget::BindStatSystem(UPRStatSystemComponent* NewStatSystem)
{
	OwnerStatSystem = NewStatSystem;
	OwnerStatSystem->OnHealthPointIsChanged.AddUObject(this, &UPRHealthPointBarWidget::UpdateHealthPointBar);
}

void UPRHealthPointBarWidget::UpdateHealthPointBar()
{
	if(OwnerStatSystem && HealthPointBar)
	{
		HealthPointBar->SetPercent(OwnerStatSystem->GetHealthPointRatio());
	}
}

void UPRHealthPointBarWidget::UpdateHealthPointBarBuffer()
{
	// HealthPointBar와 HealthPointBarBuffer의 Percent 값이 다르면
	// HealthPointBarBuffer의 Percent 값을 HealthPointerBar의 Percent 값으로 보간하여 HealthPointBar가 줄어드는 효과를 나타냅니다. 
	if(HealthPointBar && HealthPointBarBuffer && (HealthPointBar->Percent != HealthPointBarBuffer->Percent))
	{
		HealthPointBarBuffer->SetPercent(FMath::Lerp(HealthPointBarBuffer->Percent, HealthPointBar->Percent, HealthPointBarBufferSpeed));
	}
}
