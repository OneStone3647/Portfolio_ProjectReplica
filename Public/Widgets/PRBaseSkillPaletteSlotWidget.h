// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Widgets/PRBaseSkillWidget.h"
#include "PRBaseSkillPaletteSlotWidget.generated.h"

class UOverlay;
class UTextBlock;

/**
 * SkillPalette에 등록된 스킬의 정보를 출력하는 슬롯의 베이스가 되는 UserWidget 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseSkillPaletteSlotWidget : public UPRBaseSkillWidget
{
	GENERATED_BODY()

public:
	UPRBaseSkillPaletteSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	/** SkillCooldown에 관련된 위젯들을 초기화하는 함수입니다. */
	virtual void InitializeSkillCooldownWidget() override;

	/** SkillCooldown에 관련된 위젯들을 최신화하는 함수입니다. */
	virtual void UpdateSkillCooldownWidget() override;

	/** CooldownProgressMaterialInstDynamic의 Parameter를 최신화하는 함수입니다. */
	virtual void UpdateCooldownProgressParameter() override;
	
	// /** 스킬의 재사용 대기시간의 경과 시간의 비율을 반환하는 함수입니다. */
	// UFUNCTION(BlueprintCallable, Category = "PRBaseSkillPaletteSlot")
	// float GetSkillCooldownElapsedRatio() const;

	/** 스킬의 재사용 대기시간의 남은 시간을 Text로 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillPaletteSlot")
	FText GetSkillCooldownCountText() const;
	
protected:
	/** 슬롯의 외곽선들을 가진 Overlay입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillPaletteSlot", meta = (BindWidget))
	UOverlay* SkillSlotOutlines;

	/** 스킬의 재사용 대기시간을 외곽선으로 나타내는 Image입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillPaletteSlot", meta = (BindWidget))
	UImage* SkillCooldownOutline;

	/** SkillCooldownOutline에 사용하는 MaterialInstance입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseSkillPaletteSlot")
	UMaterialInterface* SkillCooldownOutlineMaterialInst;

	/** SkillCooldownOutlineMaterialInst를 바탕으로 생성한 MaterialInstanceDynamic입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseSkillPaletteSlot")
	UMaterialInstanceDynamic* SkillCooldownOutlineMaterialInstDynamic;

	/** 스킬의 재사용 대기시간을 나타내는 TextBlock입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillPaletteSlot", meta = (BindWidget))
	UTextBlock* SkillCooldownCountTextBlock;
};
