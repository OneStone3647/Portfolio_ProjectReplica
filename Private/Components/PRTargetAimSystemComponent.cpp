// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRTargetAimSystemComponent.h"
#include "Components/WidgetComponent.h"

UPRTargetAimSystemComponent::UPRTargetAimSystemComponent()
{
	UPrimitiveComponent::SetCollisionProfileName(TEXT("Target"));
	
	// LockOnWidget
	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(this);
	LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> W_PRLockOnIndicator(TEXT("/Game/Blueprints/Widgets/HUD/W_PRLockOnIndicator.W_PRLockOnIndicator_C"));
	if(W_PRLockOnIndicator.Succeeded())
	{
		LockOnWidget->SetWidgetClass(W_PRLockOnIndicator.Class);
	}
	LockOnWidget->SetDrawSize(FVector2D(25.0f, 25.0f));
	LockOnWidget->SetHiddenInGame(true);
}
