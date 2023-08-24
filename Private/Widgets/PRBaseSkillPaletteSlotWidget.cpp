// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseSkillPaletteSlotWidget.h"
#include "Skills/PRBaseSkill.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

UPRBaseSkillPaletteSlotWidget::UPRBaseSkillPaletteSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SkillSlotOutlines = nullptr;
	SkillCooldownOutline = nullptr;
	SkillCooldownOutlineMaterialInst = nullptr;
	SkillCooldownOutlineMaterialInstDynamic = nullptr;
	SkillCooldownCountTextBlock = nullptr;
}

void UPRBaseSkillPaletteSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateCooldownProgressParameter();
}

void UPRBaseSkillPaletteSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateSkillCooldownWidget();
}

void UPRBaseSkillPaletteSlotWidget::InitializeSkillCooldownWidget()
{
	Super::InitializeSkillCooldownWidget();
	
	if(SkillCooldownOutline != nullptr && SkillCooldownOutlineMaterialInst != nullptr)
	{
		SkillCooldownOutlineMaterialInstDynamic = UMaterialInstanceDynamic::Create(SkillCooldownOutlineMaterialInst, this);
		SkillCooldownOutline->SetBrushFromMaterial(SkillCooldownOutlineMaterialInstDynamic);
	}
}

void UPRBaseSkillPaletteSlotWidget::UpdateSkillCooldownWidget()
{
	if(IsValid(SourceSkill) == true)
	{
		if(SourceSkill->IsCooldown() == true)
		{
			// SkillCooldownBar->SetVisibility(ESlateVisibility::HitTestInvisible);
			// SkillCooldownOutline->SetVisibility(ESlateVisibility::HitTestInvisible);
			// SkillCooldownPercent = SourceSkill->GetSkillCooldownElapsedRatio();
			// SkillCooldownCountTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
			// SkillSlotOutlines->SetVisibility(ESlateVisibility::Collapsed);
			//
			// // 스킬의 재사용 대기시간으로 인해 이미지가 겹치지 않도록 숨깁니다.
			// SkillActivatableState->SetVisibility(ESlateVisibility::Collapsed);

			SkillCooldownOutline->SetVisibility(ESlateVisibility::HitTestInvisible);
			SkillCooldownCountTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
			SkillSlotOutlines->SetVisibility(ESlateVisibility::Collapsed);
			
			// 스택형 스킬이며 스킬을 사용할 수 있는 스택이 존재할 경우 재사용 대기시간을 나타내는 위젯만 활성화합니다.
			if(SourceSkill->GetActivateableCount() > 0)
			{
				SkillCooldownBar->SetVisibility(ESlateVisibility::Collapsed);
				SkillCooldownPercent = SourceSkill->GetSkillCooldownElapsedRatio();
			
				// 스킬이 실행할 수 있는 타입인지 판별하여 이미지의 Visibility를 설정합니다.
				if(SourceSkill->IsCanActivatableType() == true)
				{
					SkillActivatableState->SetVisibility(ESlateVisibility::Collapsed);
				}
				else
				{
					SkillActivatableState->SetVisibility(ESlateVisibility::HitTestInvisible);
				}
			}
			// 스택형 스킬이 아닐경우
			else
			{
				SkillCooldownBar->SetVisibility(ESlateVisibility::HitTestInvisible);
				SkillCooldownPercent = SourceSkill->GetSkillCooldownElapsedRatio();
			
				// 스킬의 재사용 대기시간으로 인해 이미지가 겹치지 않도록 숨깁니다.
				SkillActivatableState->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			SkillCooldownBar->SetVisibility(ESlateVisibility::Collapsed);
			SkillCooldownOutline->SetVisibility(ESlateVisibility::Collapsed);
			SkillCooldownPercent = 0.0f;
			SkillCooldownCountTextBlock->SetVisibility(ESlateVisibility::Collapsed);
			SkillSlotOutlines->SetVisibility(ESlateVisibility::HitTestInvisible);

			// 스킬이 실행할 수 있는 타입인지 판별하여 이미지의 Visibility를 설정합니다.
			if(SourceSkill->IsCanActivatableType() == true)
			{
				SkillActivatableState->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				SkillActivatableState->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}

		UpdateCooldownProgressParameter();
	}
}

void UPRBaseSkillPaletteSlotWidget::UpdateCooldownProgressParameter()
{
	Super::UpdateCooldownProgressParameter();

	if(SkillCooldownOutlineMaterialInst != nullptr)
	{
		if(SkillCooldownOutlineMaterialInstDynamic == nullptr)
		{
			InitializeSkillCooldownWidget();
		}

		if(IsValid(SkillCooldownOutlineMaterialInstDynamic) == true)
		{
			SkillCooldownOutlineMaterialInstDynamic->SetScalarParameterValue(TEXT("Percent"), SkillCooldownPercent);
		}
	}
}

// float UPRBaseSkillPaletteSlotWidget::GetSkillCooldownElapsedRatio() const
// {
// 	if(IsValid(SourceSkill) == true)
// 	{
// 		return SourceSkill->GetSkillCooldownElapsedRatio();
// 	}
//
// 	return 0.0f;
// }

FText UPRBaseSkillPaletteSlotWidget::GetSkillCooldownCountText() const
{
	if(IsValid(SourceSkill) == true)
	{
		return UKismetTextLibrary::Conv_FloatToText(SourceSkill->GetSkillCooldownRemaining(), ERoundingMode::HalfToEven, false, true,
													1, 324, 1, 1);
	}

	return FText();
}
