// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRComboCountWidget.generated.h"

class UProgressBar;
class UTextBlock;
class APRPlayerCharacter;

/**
 * 플레이어 캐릭터의 콤보 횟수를 나타내는 UserWidget 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRComboCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRComboCountWidget(const FObjectInitializer& ObjectInitializer);

#pragma region CharacterReference
public:
	/** 입력받은 PRPlayerCharacter를 바인딩하는 함수입니다. */
	void BindOwnerPRPlayerCharacter(class APRPlayerCharacter* NewPRPlayerCharacter);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterReference", meta = (AllowPrivateAccess = "true"))
	APRPlayerCharacter* OwnerPRPlayerCharacter;
#pragma endregion 

private:
	/** ComboCount를 최신화하는 함수입니다. */
	UFUNCTION()
	void UpdateComboCount(int32 NewComboCount);

	/** ComboCountTextBlock에서 사용할 Text를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ComboCount")
	FText GetComboCountText() const;

	/** ComboTimerBar의 Percent를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ComboCount")
	float GetComboTimerBarPercent() const;

private:
	/** 콤보의 유효 시간을 나타내는 ProgressBar입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ComboCount", meta = (AllowPrivateAccess = "true", BindWidget))
	UProgressBar* ComboTimerBar;

	/** 콤보의 횟수를 나타내는 TextBlock입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ComboCount", meta = (AllowPrivateAccess = "true", BindWidget))
	UTextBlock* ComboCountTextBlock;

	/** 콤보의 횟수를 나타내는 Text입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "ComboCount", meta = (AllowPrivateAccess = "true", BindWidget))
	FText ComboCountText;

	/** Widget을 흔드는 WidgetAnimation입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ComboCount", Transient, meta = (AllowPrivateAccess = "true", BindWidgetAnim))
	UWidgetAnimation* ShakeAnimation;
};
