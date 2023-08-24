// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseInGameInterfaceWidget.h"
#include "Characters/PRPlayerCharacter.h"
#include "PRPlayerController.h"

UPRBaseInGameInterfaceWidget::UPRBaseInGameInterfaceWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPRBaseInGameInterfaceWidget::BindOwnerPRPlayerCharacter_Implementation(const APRPlayerCharacter* NewPRPlayerCharacter)
{
}

void UPRBaseInGameInterfaceWidget::BindOwnerPRPlayerController_Implementation(const APRPlayerController* NewPRPlayerController)
{
}
