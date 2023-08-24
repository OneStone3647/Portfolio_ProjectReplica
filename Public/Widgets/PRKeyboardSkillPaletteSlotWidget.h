// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Widgets/PRBaseSkillPaletteSlotWidget.h"
#include "PRKeyboardSkillPaletteSlotWidget.generated.h"

/**
 * 키보드를 사용할 때 사용하는 SkillPalette의 슬롯입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRKeyboardSkillPaletteSlotWidget : public UPRBaseSkillPaletteSlotWidget
{
	GENERATED_BODY()

public:
	UPRKeyboardSkillPaletteSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
public:
	/** InputKeyIcon을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "KeyboardSkillPaletteSlot")
	void InitializeInputKeyIcon();

private:
	/** 키보드 입력키의 아이콘입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KeyboardSkillPaletteSlot", meta = (AllowPrivateAccess = "true", BindWidget))
	UImage* InputKeyIcon;

	/** 키보드 입력키 아이콘의 이미지입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "KeyboardSkillPaletteSlot", meta = (AllowPrivateAccess = "true"))
	UTexture2D* InputKeyIconImage;

	/** 불러올 액션매핑의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KeyboardSkillPaletteSlot", meta = (AllowPrivateAccess = "true"))
	FName ActionMappingName;

public:
	/** 입력받은 인자로 InputKeyIconImage를 설정하는 함수입니다. */
	void SetInputKeyIconImage(UTexture2D* NewInputKeyIconImage);
	
	/** ActionMappingName을 반환하는 함수입니다. */
	FName GetActionMappingName() const;
};
