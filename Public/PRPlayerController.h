// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

class UPRHUD;
class UPRInGameHUD;

UENUM(BlueprintType)
enum class EPRGamepadInputKey : uint8
{
	GamepadInputKey_FaceButtonLeft			UMETA(DisplayName = "GamepadFaceButtonLeft"),
	GamepadInputKey_FaceButtonTop			UMETA(DisplayName = "GamepadFaceButtonTop"),
	GamepadInputKey_FaceButtonRight			UMETA(DisplayName = "GamepadFaceButtonRight"),
	GamepadInputKey_FaceButtonBottom		UMETA(DisplayName = "GamepadFaceButtonBottom"),
	GamepadInputKey_LeftShoulder			UMETA(DisplayName = "GamepadLeftShoulder"),
	GamepadInputKey_LeftTrigger				UMETA(DisplayName = "GamepadLeftTrigger"),
	GamepadInputKey_RightShoulder			UMETA(DisplayName = "GamepadRightShoulder"),
	GamepadInputKey_RightTrigger			UMETA(DisplayName = "GamepadRightTrigger")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputGamepadKeyDelegate, EPRGamepadInputKey, NewGamepadInputKey, bool, bIsPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoldInputKeyDelegate);

/**
 * 플레이어가 사용하는 컨트롤러입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APRPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

#pragma region InputDevice
public:
	UFUNCTION(BlueprintCallable, Category = "InputDevice")
	bool IsGamepadInput() const;
	
private:
	/** 입력장치가 키보드/마우스인지 게임패드인지 확인하고 bIsGamepadInput를 설정하는 함수입니다. */
	void DetermineInputDeviceDetails(FKey Key);

private:
	/** 게임패드 입력인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InputDevice", meta = (AllowPrivateAccess = "true"))
	bool bIsGamepadInput;
#pragma endregion

#pragma region CreateWidget
public:
	/**	입력받은 인자로 Widget을 생성하고 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CreateWidget")
	class UUserWidget* CreateWidgetFromClassReference(TSubclassOf<class UUserWidget> DrawWidgetClass);
#pragma endregion 

#pragma region InGameHUD
private:
	/** InGameHUD로 사용할 UserWidget의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> InGameHUDClass;

	/** 플레이어가 인게임에서 사용하는 HUD입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UPRInGameHUD* InGameHUD;

public:
	/** InGameHUD를 반환하는 함수입니다. */
	class UPRInGameHUD* GetInGameHUD() const;
#pragma endregion

#pragma region InputKeyIcon
private:
	/** 키보드 입력키의 아이콘을 보관한 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputKeyIcon", meta = (AllowPrivateAccess = "true"))
	UDataTable* KeyboardInputKeyIconDataTable;

	/** 게임패드 입력키의 아이콘을 보관한 데이터 테이블입다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputKeyIcon", meta = (AllowPrivateAccess = "true"))
	UDataTable* GamepadInputKeyIconDataTable;

public:
	/** KeyboardInputKeyIconDataTable을 반환하는 함수입니다. */
	UDataTable* GetKeyboardInputKeyIconDataTable() const;

	/** GamepadInputKeyIconDataTable을 반환하는 함수입니다. */
	UDataTable* GetGamepadInputKeyIconDataTable() const;
#pragma endregion

#pragma region InputKeyUI
protected:
	/**
	 * 게임패드의 입력이 있을 때 실행하는 함수입니다.
	 *
	 * @param NewGamepadInputKey 입력된 게임패드의 키입니다.
	 * @param bIsPressed 눌러졌을 경우 true, 떼어졌을 경우 false를 입력합니다.
	 */
	void InputGamepadKey(EPRGamepadInputKey NewGamepadInputKey, bool bIsPressed);

public:
	/** 게임패드의 입력이 있을 때 호출하는 Delegate입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "PRPlayerController|InputKeyUI")
	FOnInputGamepadKeyDelegate OnInputGamepadKeyDelegate;
#pragma endregion

#pragma region HoldInput
public:
	/** 홀드 입력의 시간을 최신화하는 함수입니다. */
	void UpdateHoldInputKeyTime(float Value);

	/**
	 * HoldInputKey를 초기화하는 함수입니다.
	 *
	 * @param NewHoldInputKey 홀드 입력의 키로 설정할 키입니다.
	 * @param NewHoldableInputKeyTime 홀드 입력에 대한 기능의 실행에 필요한 홀드 키의 홀드 시간입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	void InitializeHoldInputKey(const FKey& NewHoldInputKey, float NewHoldableInputKeyTime);

	bool IsExecuteHoldInput() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPlayerController|InputCheck", meta = (AllowPrivateAccess = "true"))
	bool bExecuteHoldInput;
	
	/** 홀드 입력의 키입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPlayerController|InputCheck", meta = (AllowPrivateAccess = "true"))
	FKey HoldInputKey;
	
	/** 홀드 입력의 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPlayerController|InputCheck", meta = (AllowPrivateAccess = "true"))
	float HoldInputKeyTime;

	/** 홀드 입력에 대한 기능의 실행에 필요한 홀드 키의 홀드 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRPlayerController|InputCheck", meta = (AllowPrivateAccess = "true"))
	float HoldableInputKeyTime;

public:
	/** 홀드 입력이 있을 때 호출하는 Delegate입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "PRPlayerController|InputCheck")
	FOnHoldInputKeyDelegate OnHoldInputKeyDelegate;
	
public:
	void SetExecuteHoldInput(bool bNewExecuteHoldInput);
	
	/** 입력받은 인자로 HoldableInputKeyTime을 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	void SetHoldableInputKeyTime(float NewHoldableInputKeyTime);
#pragma endregion 

#pragma region InputCheck
public:
	/**
	 * 입력한 인자에 해당하는 액션 매핑의 키를 눌러졌는지 판별하는 함수입니다.
	 *
	 * @param ActionMappingName 키를 눌러졌는지 판별할 액션 매핑의 이름입니다.
	 * @return 액션 매핑의 키를 눌러졌을 경우 true, 안눌러졌을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	bool IsPressedActionKeyMapping(FName ActionMappingName) const;
	
	/**
	 * 입력한 인자에 해당하는 액션 매핑의 키를 떼어졌는지 판별하는 함수입니다.
	 *
	 * @param ActionMappingName 키를 떼어졌는지 판별할 액션 매핑의 이름입니다.
	 * @return 액션 매핑의 키를 떼어졌을 경우 true, 안떼어졌을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	bool IsReleasedActionKeyMapping(FName ActionMappingName) const;

	/**
	 * 입력한 인자에 해당하는 축 매핑의 키가 눌러졌는지 판별하는 함수입니다.
	 *
	 * @param AxisMappingName 키를 눌러졌는지 판별할 축 매핑의 이름입니다.
	 * @return 축 매핑의 키를 눌러졌을 경우 true, 안눌러졌을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	bool IsPressedAxisKeyMapping(FName AxisMappingName) const;

	/**
	 * 입력한 인자에 해당하는 축 매핑의 키를 떼어졌는지 판별하는 함수입니다.
	 *
	 * @param AxisMappingName 키를 떼어졌는지 판별할 축 매핑의 이름입니다.
	 * @return 축 매핑의 키를 떼어졌을 경우 true, 안떼어졌을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	bool IsReleasedAxisKeyMapping(FName AxisMappingName) const;

	/**
	 * 입력한 인자에 해당하는 액션 매핑에서 눌러진 키를 반환하는 함수입니다.
	 *
	 * @param ActionMappingName 키를 눌러졌는지 판별할 액션 매핑의 이름입니다.
	 * @return 액션 매핑에서 눌러진 키를 반환합니다. 눌러진 키가 없을 경우 기본 생성자를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	void GetPressedActionKeyToMapping(FName ActionMappingName, FKey& PressedActionKey) const;

	/**
	 * 입력한 인자에 해당하는 액션 매핑에서 떼어진 키를 반환하는 함수입니다. 
	 *
	 * @param ActionMappingName 키를 떼어졌는지 판별할 액션 매핑의 이름입니다.
	 * @return 액션 매핑에서 떼어진 키를 반환합니다. 떼어진 키가 없을 경우 기본 생성자를 반환합니다.
	 */
	void GetReleasedActionKeyToMapping(FName ActionMappingName, FKey& ReleasedActionKey) const;

	/**
	 * 입력한 인자에 해당하는 액션 매핑에 해당하는 키들을 Array로 반환하는 함수입니다.
	 *
	 * @param ActionMappingName 가져올 액션 매핑의 이름입니다.
	 * @return 액션 매핑에 해당하는 키들을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerController|InputCheck")
	void GetActionKeysToMapping(FName ActionMappingName, TArray<FKey>& ActionKeys) const;
#pragma endregion 
};

