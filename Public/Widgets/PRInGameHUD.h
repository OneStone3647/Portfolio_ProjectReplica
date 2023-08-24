// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRInGameHUD.generated.h"

class UPRHealthPointBarWidget;
class UPRComboCountWidget;
class UPRInteractWidget;
// class UPRGamepadSkillPaletteWidget;
// class UPRKeyboardSkillPaletteWidget;
class UPRBaseInGameInterfaceWidget;

/**
 * 플레이어가 사용하는 HUD입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRInGameHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRInGameHUD(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

#pragma region InputDevice
private:
	/** 게임패드 입력인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InputDevice", meta = (AllowPrivateAccess = "true"))
	bool bIsGamepadInput;	

public:
	/** 입력받은 인자로 bIsGamepadInput를 설정하는 함수입니다. */
	UFUNCTION()
	void SetIsGamepadInput(bool bNewIsGamepadInput);
#pragma endregion 

public:
	/** HUD에서 사용하는 모든 위젯을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
	void InitializeWidgets();
	virtual void InitializeWidgets_Implementation();

private:
	/** 플레이어 캐릭터의 체력바입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	UPRHealthPointBarWidget* HealthPointBarWidget;

	/** 콤보 횟수를 나타내는 위젯입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	UPRComboCountWidget* ComboCountWidget;

	/** 상호작용 목록을 나타내는 위젯입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	UPRInteractWidget* InteractWidget;

	// /** 게임패드를 사용할 때 사용하는 SkillPalette입니다. */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	// UPRGamepadSkillPaletteWidget* GamepadSkillPaletteWidget;
	//
	// /** 키보드를 사용할 때 사용하는 SkillPalette입니다. */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	// UPRKeyboardSkillPaletteWidget* KeyboardSkillPaletteWidget;

	/** 게임패드를 사용할 때 사용하는 인게임 인터페이스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	UPRBaseInGameInterfaceWidget* GamepadInGameInterfaceWidget;

	/** 키보드를 사용할 때 사용하는 인게임 인터페이스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true", BindWidget))
	UPRBaseInGameInterfaceWidget* KeyboardInGameInterfaceWidget;

public:
	/** ComboCountWidget을 반환하는 함수입니다. */
	class UPRComboCountWidget* GetComboCountWidget() const;

	/** InteractWidget을 반환하는 함수입니다. */
	class UPRInteractWidget* GetInteractWidget() const;
};
