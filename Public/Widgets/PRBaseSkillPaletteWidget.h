// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRBaseSkillPaletteWidget.generated.h"

class APRPlayerCharacter;

/**
 * 플레이어 캐릭터의 스킬 정보를 HUD에 표시하는 SkillPalette 위젯의 기본 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseSkillPaletteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRBaseSkillPaletteWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	/**
	 * 스킬 슬롯에 해당하는 스킬을 바인딩하는 함수입니다.
	 *
	 * @param NewPRPlayerCharacter 바인딩할 스킬을 가진 플레이어 캐릭터입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRBaseSkillPalette")
	void BindSkillSlot(const class APRPlayerCharacter* NewPRPlayerCharacter);
	virtual void BindSkillSlot_Implementation(const class APRPlayerCharacter* NewPRPlayerCharacter);

protected:
	/** 스킬 슬롯에 바인된 스킬의 정보를 나타내는 PaletteSlotWidget을 보관한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseSkillPalette")
	TArray<class UPRBaseSkillPaletteSlotWidget*> SkillPaletteSlotWidgets;

#pragma region CharacterReference
public:
	/** 입력받은 PRPlayerCharacter를 바인딩하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseSkillPalette")
	virtual void BindPROwnerPlayerCharacter(class APRPlayerCharacter* NewPRPlayerCharacter);

protected:
	/** 이 SkillPalette를 사용하는 플레이어 캐릭터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterReference")
	APRPlayerCharacter* PROwnerPlayerCharacter;	
#pragma endregion 
};
