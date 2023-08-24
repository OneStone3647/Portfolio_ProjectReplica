// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRBaseSkillWidget.generated.h"

class UPRBaseSkill;
class UImage;

/**
 * 스킬의 정보를 나타내는 위젯의 기본이 되는 UserWidget 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseSkillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRBaseSkillWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** 슬롯을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void InitializeSlot();

	/**
	 * 인자로 받은 스킬을 SourceSkill로 설정하고 필요한 정보를 바인딩하는 함수입니다.
	 *
	 * @param NewSourceSkill SourceSkill로 설정할 스킬입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void BindSkill(class UPRBaseSkill* NewSourceSkill);
	
protected:
	/** SkillIcon을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void InitializeSkillIcon();
	
	/** SkillCooldown에 관련된 위젯들을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void InitializeSkillCooldownWidget();

	/** SkillCooldown에 관련된 위젯들을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void UpdateSkillCooldownWidget();

	/** CooldownProgressMaterialInstDynamic의 Parameter를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillWidget")
	virtual void UpdateCooldownProgressParameter();

protected:
	/** 술롯이 정보를 출력할 스킬입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseSkillWidget")
	UPRBaseSkill* SourceSkill;
	
	/** 슬롯의 이미지입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillWidget", meta = (BindWidget))
	UImage* SkillSlot;

	/** 스킬의 아이콘입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillWidget", meta = (BindWidget))
	UImage* SkillIcon;

	/** 스킬 아이콘의 이미지입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PRBaseSkillWidget")
	UTexture2D* SkillIconImage;
	
	/** SkillIcon의 크기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillWidget")
	FVector2D SkillIconImageSize;

	/** 스킬의 재사용 대기시간의 경과 시간의 비율입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseSkillWidget", meta = (BindWidget))
	float SkillCooldownPercent;
	
	/** 스킬의 재사용 대기시간을 나타내는 Image입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillWidget", meta = (BindWidget))
	UImage* SkillCooldownBar;

	/** SkillCooldownBar에 사용하는 MaterialInstance입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseSkillWidget")
	UMaterialInterface* SkillCooldownBarMaterialInst;

	/** SkillCooldownBarMaterialInst를 바탕으로 생성한 MaterialInstanceDynamic입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseSkillWidget")
	UMaterialInstanceDynamic* SkillCooldownBarMaterialInstDynamic;

	/** 스킬의 실행가능 여부를 나타내는 Image입니다. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PRBaseSkillWidget", meta = (BindWidget))
	UImage* SkillActivatableState;
};
