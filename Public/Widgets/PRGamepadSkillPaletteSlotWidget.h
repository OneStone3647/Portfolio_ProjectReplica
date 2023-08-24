// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Widgets/PRBaseSkillPaletteSlotWidget.h"
#include "PRGamepadSkillPaletteSlotWidget.generated.h"

/**
 * 키보드를 사용할 때 사용하는 SkillPalette의 슬롯입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRGamepadSkillPaletteSlotWidget : public UPRBaseSkillPaletteSlotWidget
{
	GENERATED_BODY()

public:
	UPRGamepadSkillPaletteSlotWidget(const FObjectInitializer& ObjectInitializer);
};
