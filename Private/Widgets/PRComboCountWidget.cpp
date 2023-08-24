// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRComboCountWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Characters/PRPlayerCharacter.h"
#include "Kismet/KismetTextLibrary.h"


UPRComboCountWidget::UPRComboCountWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// CharacterReference
	OwnerPRPlayerCharacter = nullptr;
	
	ComboTimerBar = nullptr;
	ComboCountTextBlock = nullptr;
	ComboCountText = FText();
	ShakeAnimation = nullptr;
}

#pragma region CharacterReference
void UPRComboCountWidget::BindOwnerPRPlayerCharacter(APRPlayerCharacter* NewPRPlayerCharacter)
{
	if(IsValid(NewPRPlayerCharacter) == true)
	{
		OwnerPRPlayerCharacter = NewPRPlayerCharacter;
		OwnerPRPlayerCharacter->OnComboCountChangedDelegate.AddDynamic(this, &UPRComboCountWidget::UpdateComboCount);
	}
}
#pragma endregion 

void UPRComboCountWidget::UpdateComboCount(int32 NewComboCount)
{
	ComboCountText = UKismetTextLibrary::Conv_IntToText(NewComboCount);
	PlayAnimation(ShakeAnimation);
}

FText UPRComboCountWidget::GetComboCountText() const
{
	return ComboCountText;
}

float UPRComboCountWidget::GetComboTimerBarPercent() const
{
	if(OwnerPRPlayerCharacter != nullptr)
	{
		return OwnerPRPlayerCharacter->GetComboCountResetTimeRatio();
	}

	return 0.0f;
}
