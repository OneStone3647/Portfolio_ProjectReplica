// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseSkillPaletteWidget.h"
#include "Widgets/PRBaseSkillPaletteSlotWidget.h"
#include "Characters/PRPlayerCharacter.h"

UPRBaseSkillPaletteWidget::UPRBaseSkillPaletteWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PROwnerPlayerCharacter = nullptr;
	SkillPaletteSlotWidgets.Empty();
}

void UPRBaseSkillPaletteWidget::BindSkillSlot_Implementation(const APRPlayerCharacter* NewPRPlayerCharacter)
{
}

void UPRBaseSkillPaletteWidget::BindPROwnerPlayerCharacter(APRPlayerCharacter* NewPROwnerPlayerCharacter)
{
	if(IsValid(NewPROwnerPlayerCharacter) == true)
	{
		PROwnerPlayerCharacter = NewPROwnerPlayerCharacter;

		BindSkillSlot(PROwnerPlayerCharacter);
	}
}

