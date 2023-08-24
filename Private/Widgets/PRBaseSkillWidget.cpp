// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRBaseSkillWidget.h"
#include "Skills/PRBaseSkill.h"
#include "Components/Image.h"

UPRBaseSkillWidget::UPRBaseSkillWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SourceSkill = nullptr;
	SkillSlot = nullptr;
	SkillIcon = nullptr;
	SkillIconImage = nullptr;
	SkillIconImageSize = FVector2D(32.0f, 32.0f);
	SkillCooldownPercent = 0.0f;
	SkillCooldownBar = nullptr;
	SkillCooldownBarMaterialInst = nullptr;
	SkillCooldownBarMaterialInstDynamic = nullptr;
	SkillActivatableState = nullptr;
}

void UPRBaseSkillWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateCooldownProgressParameter();
}

void UPRBaseSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateSkillCooldownWidget();
}

void UPRBaseSkillWidget::InitializeSlot()
{
	SourceSkill = nullptr;
}

void UPRBaseSkillWidget::BindSkill(UPRBaseSkill* NewSourceSkill)
{
	if(IsValid(NewSourceSkill) == true)
	{
		SourceSkill = NewSourceSkill;

		InitializeSkillIcon();
		InitializeSkillCooldownWidget();
	}
}

void UPRBaseSkillWidget::InitializeSkillIcon()
{
	if(IsValid(SourceSkill) == true)
	{
		SkillIcon->SetBrushFromTexture(SourceSkill->GetSkillInfo().SkillIcon);
		SkillIcon->SetBrushSize(SkillIconImageSize);
	}
}

void UPRBaseSkillWidget::InitializeSkillCooldownWidget()
{
	if(SkillCooldownBar != nullptr && SkillCooldownBarMaterialInst != nullptr)
	{
		SkillCooldownBarMaterialInstDynamic = UMaterialInstanceDynamic::Create(SkillCooldownBarMaterialInst, this);
		SkillCooldownBar->SetBrushFromMaterial(SkillCooldownBarMaterialInstDynamic);
	}
}

void UPRBaseSkillWidget::UpdateSkillCooldownWidget()
{
	if(IsValid(SourceSkill) == true)
	{
		if(SourceSkill->IsCooldown() == true)
		{
			SkillCooldownBar->SetVisibility(ESlateVisibility::HitTestInvisible);
			SkillCooldownPercent = SourceSkill->GetSkillCooldownElapsedRatio();

			// 스킬의 재사용 대기시간으로 인해 이미지가 겹치지 않도록 숨깁니다.
			SkillActivatableState->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SkillCooldownBar->SetVisibility(ESlateVisibility::Collapsed);
			SkillCooldownPercent = 0.0f;

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

void UPRBaseSkillWidget::UpdateCooldownProgressParameter()
{
	if(SkillCooldownBarMaterialInst != nullptr)
	{
		if(SkillCooldownBarMaterialInstDynamic == nullptr)
		{
			InitializeSkillCooldownWidget();
		}

		if(IsValid(SkillCooldownBarMaterialInstDynamic) == true)
		{
			SkillCooldownBarMaterialInstDynamic->SetScalarParameterValue(TEXT("Percent"), SkillCooldownPercent);
		}
	}
}
