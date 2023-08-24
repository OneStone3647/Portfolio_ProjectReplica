// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Widgets/PRBaseSkillPaletteWidget.h"
#include "PRGamepadSkillPaletteWidget.generated.h"

/**
 * 게임패드를 사용할 때 사용하는 SkillPalette 위젯의 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRGamepadSkillPaletteWidget : public UPRBaseSkillPaletteWidget
{
	GENERATED_BODY()

public:
	UPRGamepadSkillPaletteWidget(const FObjectInitializer& ObjectInitializer);

protected:
	/**
	 * 입력받은 인자에 따라 SkillPalette의 펼치거나 접는 함수입니다.
	 *
	 * @param bNewIsOpenSkillPalette true일 경우 SkillPalette를 펼치고, false일 경우 SkillPalette를 접습니다..
	 */
	UFUNCTION()
	void OpenSkillPalette(bool bNewIsOpenSkillPalette);

private:
	/** SkillPalette가 펼쳤는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRGamepadSkillPalette", meta = (AllowPrivateAccess = "true"))
	bool bIsOpenSkillPalette;

	/** SkillPalette가 펼쳐지는 WidgetAnimation입니다. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "SkillPalette", meta = (AllowPrivateAccess = "true", BindWidgetAnim))
	UWidgetAnimation* OpenSkillPaletteAnim;
	
#pragma region CharacterReference
public:
	/** 입력받은 PRPlayerCharacter를 바인딩하는 함수입니다. */
	virtual void BindPROwnerPlayerCharacter(class APRPlayerCharacter* NewPRPlayerCharacter) override;
#pragma endregion 
};
