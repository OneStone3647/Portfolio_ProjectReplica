// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRGamepadSkillPaletteWidget.h"

#include "Characters/PRPlayerCharacter.h"

UPRGamepadSkillPaletteWidget::UPRGamepadSkillPaletteWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsOpenSkillPalette = false;
	OpenSkillPaletteAnim = nullptr;
}

void UPRGamepadSkillPaletteWidget::OpenSkillPalette(bool bNewIsOpenSkillPalette)
{
	bIsOpenSkillPalette = bNewIsOpenSkillPalette;
	if(bIsOpenSkillPalette)
	{
		PlayAnimationForward(OpenSkillPaletteAnim);
	}
	else
	{
		PlayAnimationReverse(OpenSkillPaletteAnim);
	}
}

void UPRGamepadSkillPaletteWidget::BindPROwnerPlayerCharacter(APRPlayerCharacter* NewPROwnerPlayerCharacter)
{
	Super::BindPROwnerPlayerCharacter(NewPROwnerPlayerCharacter);

	if(IsValid(PROwnerPlayerCharacter) == true)
	{
		PROwnerPlayerCharacter->OnOpenSkillPaletteDelegate.AddDynamic(this, &UPRGamepadSkillPaletteWidget::OpenSkillPalette);
	}
}
