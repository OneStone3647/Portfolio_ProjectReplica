// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/PRTargetAimSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/PRHealthPointBarWidget.h"

APRAICharacter::APRAICharacter()
{
	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AICharacter"));	
	
	// TargetAimSystem
	TargetAimSystem = CreateDefaultSubobject<UPRTargetAimSystemComponent>(TEXT("TargetAimSystem"));
	TargetAimSystem->SetupAttachment(RootComponent);

	// HealthPointBarWidget
	HealthPointBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthPointBarWidget"));
	HealthPointBarWidget->SetupAttachment(GetMesh());
	HealthPointBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HealthPointBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthPointBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	// HealthPointBarWidget->SetHiddenInGame(true);
}

void APRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitHealthPointBarWidget();
}

#pragma region TargetAimSystem
void APRAICharacter::SelectedTarget(bool bFlag)
{
	TargetAimSystem->GetLockOnWidget()->SetHiddenInGame(!bFlag);
}

FVector APRAICharacter::GetTargetAimLocation() const
{
	return GetTargetAimSystem()->GetComponentLocation();
}

void APRAICharacter::InitHealthPointBarWidget()
{
	UPRHealthPointBarWidget* HealthPointBar = Cast<UPRHealthPointBarWidget>(HealthPointBarWidget->GetUserWidgetObject());
	if(HealthPointBar)
	{
		HealthPointBar->BindStatSystem(GetStatSystem());
	}
}
#pragma endregion 
