// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Widgets/PRBaseSkillPaletteWidget.h"
#include "PRKeyboardSkillPaletteWidget.generated.h"

/**
 * 키보드를 사용할 때 사용하는 SkillPalette 위젯의 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRKeyboardSkillPaletteWidget : public UPRBaseSkillPaletteWidget
{
	GENERATED_BODY()

public:
	UPRKeyboardSkillPaletteWidget(const FObjectInitializer& ObjectInitializer);
};
