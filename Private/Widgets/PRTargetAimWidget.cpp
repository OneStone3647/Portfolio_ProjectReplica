// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRTargetAimWidget.h"
#include "Components/Image.h"

UPRTargetAimWidget::UPRTargetAimWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LockOnIndicatorImage = nullptr;
}

void UPRTargetAimWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LockOnIndicatorImage = Cast<UImage>(GetWidgetFromName(TEXT("LockOnIndicatorImage")));
}
