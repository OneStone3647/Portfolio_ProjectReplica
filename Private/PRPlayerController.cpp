// Fill out your copyright notice in the Description page of Project Settings.


#include "PRPlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "Widgets/PRInGameHUD.h"

APRPlayerController::APRPlayerController()
{
	// InputDevice
	bIsGamepadInput = false;

	// InGameHUD
	InGameHUDClass = nullptr;
	InGameHUD = nullptr;

	// InputKeyIcon
	KeyboardInputKeyIconDataTable = nullptr;
	GamepadInputKeyIconDataTable = nullptr;

	// HoldInput
	HoldInputKey = FKey();
	HoldInputKeyTime = 0.0f;
	HoldableInputKeyTime = 0.0f;
}

void APRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임을 시작하면 ViewPort에 마우스를 가둡니다.
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	UPRInGameHUD* NewInGameHUD = Cast<UPRInGameHUD>(CreateWidgetFromClassReference(InGameHUDClass));
	if(NewInGameHUD)
	{
		InGameHUD = NewInGameHUD;
		InGameHUD->AddToViewport();
	}
}

void APRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("AnyInput", IE_Pressed, this, &APRPlayerController::DetermineInputDeviceDetails);

	InputComponent->BindAxis("HoldInput", this, &APRPlayerController::UpdateHoldInputKeyTime);

	// InputKey UI 관련 ActionInput
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonLeft", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonLeft, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonLeft", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonLeft, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonUp", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonTop, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonUp", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonTop, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonRight", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonRight, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonRight", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonRight, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonBottom", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonBottom, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadFaceButtonBottom", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_FaceButtonBottom, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadLeftShoulder", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_LeftShoulder, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadLeftShoulder", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_LeftShoulder, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadLeftTrigger", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_LeftTrigger, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadLeftTrigger", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_LeftTrigger, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadRightShoulder", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_RightShoulder, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadRightShoulder", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_RightShoulder, false);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadRightTrigger", IE_Pressed, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_RightTrigger, true);
	InputComponent->BindAction<TDelegate<void(EPRGamepadInputKey, bool)>>("GamepadRightTrigger", IE_Released, this, &APRPlayerController::InputGamepadKey, EPRGamepadInputKey::GamepadInputKey_RightTrigger, false);
	
	for(int Index = 0; Index < InputComponent->GetNumActionBindings(); ++Index)
	{
		// 입력의 우선권을 해제합니다.
		InputComponent->GetActionBinding(Index).bConsumeInput = false;
	}

	for(int Index = 0; Index < InputComponent->AxisBindings.Num(); ++ Index)
	{
		// 입력의 우선권을 해제합니다.
		InputComponent->AxisBindings[Index].bConsumeInput = false;
	}
}

#pragma region InputDevice
bool APRPlayerController::IsGamepadInput() const
{
	return bIsGamepadInput;
}

void APRPlayerController::DetermineInputDeviceDetails(FKey Key)
{
	if(Key.IsGamepadKey() == true)
	{
		if(!bIsGamepadInput)
		{
			bIsGamepadInput = true;
			if(InGameHUD != nullptr)
			{
				InGameHUD->SetIsGamepadInput(bIsGamepadInput);
			}
		}
	}
	else
	{
		if(bIsGamepadInput)
		{
			bIsGamepadInput = false;
			if(InGameHUD != nullptr)
			{
				InGameHUD->SetIsGamepadInput(bIsGamepadInput);
			}
		}
	}
}
#pragma endregion 

#pragma region CraeteWidget
UUserWidget* APRPlayerController::CreateWidgetFromClassReference(TSubclassOf<UUserWidget> DrawWidgetClass)
{
	if(DrawWidgetClass != nullptr)
	{
		UUserWidget* NewDrawWidget = CreateWidget(this, DrawWidgetClass);
		if(NewDrawWidget)
		{
			return NewDrawWidget;
		}
	}

	return nullptr;
}
#pragma endregion

#pragma region InGameHUD
UPRInGameHUD* APRPlayerController::GetInGameHUD() const
{
	return InGameHUD;
}
#pragma endregion 

#pragma region InputKeyIcon
UDataTable* APRPlayerController::GetKeyboardInputKeyIconDataTable() const
{
	return KeyboardInputKeyIconDataTable;
}

UDataTable* APRPlayerController::GetGamepadInputKeyIconDataTable() const
{
	return GamepadInputKeyIconDataTable;
}
#pragma endregion

#pragma region InputKeyUI
void APRPlayerController::InputGamepadKey(EPRGamepadInputKey NewGamepadInputKey, bool bIsPressed)
{
	if(OnInputGamepadKeyDelegate.IsBound() == true)
	{
		OnInputGamepadKeyDelegate.Broadcast(NewGamepadInputKey, bIsPressed);
	}
}
#pragma endregion

#pragma region HoldInput
void APRPlayerController::UpdateHoldInputKeyTime(float Value)
{
	HoldInputKeyTime = GetInputKeyTimeDown(HoldInputKey);
	if(HoldableInputKeyTime != 0.0f
		&& bExecuteHoldInput == true
		&& HoldInputKeyTime >= HoldableInputKeyTime
		&& OnHoldInputKeyDelegate.IsBound() == true)
	{
		OnHoldInputKeyDelegate.Broadcast();
	}
}

void APRPlayerController::InitializeHoldInputKey(const FKey& NewHoldInputKey, float NewHoldableInputKeyTime)
{
	TArray<FInputAxisKeyMapping> InputAxisKeyMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName("HoldInput", InputAxisKeyMappings);
	for(const FInputAxisKeyMapping& InputAxisKeyMapping : InputAxisKeyMappings)
	{
		UInputSettings::GetInputSettings()->RemoveAxisMapping(InputAxisKeyMapping);
	}
	
	HoldInputKey = NewHoldInputKey;
	FInputAxisKeyMapping HoldInputKeyMapping = FInputAxisKeyMapping("HoldInput", HoldInputKey, 1.0f);
	UInputSettings::GetInputSettings()->AddAxisMapping(HoldInputKeyMapping);

	SetHoldableInputKeyTime(NewHoldableInputKeyTime);
	SetExecuteHoldInput(false);
}

bool APRPlayerController::IsExecuteHoldInput() const
{
	return bExecuteHoldInput;
}

void APRPlayerController::SetExecuteHoldInput(bool bNewExecuteHoldInput)
{
	bExecuteHoldInput = bNewExecuteHoldInput;
}

void APRPlayerController::SetHoldableInputKeyTime(float NewHoldableInputKeyTime)
{
	HoldableInputKeyTime = NewHoldableInputKeyTime;
}
#pragma endregion 

#pragma region InputCheck
bool APRPlayerController::IsPressedActionKeyMapping(FName ActionMappingName) const
{
	TArray<FInputActionKeyMapping> ActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
	{
		if(WasInputKeyJustPressed(ActionMapping.Key) == true)
		{
			return true;
		}
	}

	return false;
}

bool APRPlayerController::IsReleasedActionKeyMapping(FName ActionMappingName) const
{
	TArray<FInputActionKeyMapping> ActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
	{
		if(WasInputKeyJustReleased(ActionMapping.Key) == true)
		{
			return true;
		}
	}

	return false;
}

bool APRPlayerController::IsPressedAxisKeyMapping(FName AxisMappingName) const
{
	TArray<FInputAxisKeyMapping> AxisMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName(AxisMappingName, AxisMappings);
	for(const FInputAxisKeyMapping& AxisMapping : AxisMappings)
	{
		if(WasInputKeyJustPressed(AxisMapping.Key) == true)
		{
			return true;
		}
	}

	return false;
}

bool APRPlayerController::IsReleasedAxisKeyMapping(FName AxisMappingName) const
{
	TArray<FInputAxisKeyMapping> AxisMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName(AxisMappingName, AxisMappings);
	for(const FInputAxisKeyMapping& AxisMapping : AxisMappings)
	{
		if(WasInputKeyJustReleased(AxisMapping.Key) == true)
		{
			return true;
		}
	}

	return false;
}

void APRPlayerController::GetPressedActionKeyToMapping(FName ActionMappingName, FKey& PressedActionKey) const
{
	TArray<FInputActionKeyMapping> ActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
	{
		if(WasInputKeyJustPressed(ActionMapping.Key) == true)
		{
			PressedActionKey = ActionMapping.Key;
		}
	}
}

void APRPlayerController::GetReleasedActionKeyToMapping(FName ActionMappingName, FKey& ReleasedActionKey) const
{
	TArray<FInputActionKeyMapping> ActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
	{
		if(WasInputKeyJustReleased(ActionMapping.Key) == true)
		{
			ReleasedActionKey = ActionMapping.Key;
		}
	}
}

void APRPlayerController::GetActionKeysToMapping(FName ActionMappingName, TArray<FKey>& ActionKeys) const
{
	TArray<FInputActionKeyMapping> ActionMappings;
	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
	{
		ActionKeys.Emplace(ActionMapping.Key);
	}
}

// FKey APRPlayerController::GetPressedActionKeyToMapping(FName ActionMappingName) const
// {
// 	TArray<FInputActionKeyMapping> ActionMappings;
// 	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
// 	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
// 	{
// 		if(WasInputKeyJustPressed(ActionMapping.Key) == true)
// 		{
// 			return ActionMapping.Key;
// 		}
// 	}
//
// 	return FKey();
// }
//
// FKey APRPlayerController::GetReleasedActionKeyToMapping(FName ActionMappingName) const
// {
// 	TArray<FInputActionKeyMapping> ActionMappings;
// 	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
// 	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
// 	{
// 		if(WasInputKeyJustReleased(ActionMapping.Key) == true)
// 		{
// 			return ActionMapping.Key;
// 		}
// 	}
//
// 	return FKey();
// }
//
// TArray<FKey> APRPlayerController::GetActionKeysToMapping(FName ActionMappingName) const
// {
// 	TArray<FKey> ActionKeys;
// 	TArray<FInputActionKeyMapping> ActionMappings;
// 	UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
// 	for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
// 	{
// 		ActionKeys.Emplace(ActionMapping.Key);
// 	}
//
// 	return ActionKeys;
// }
#pragma endregion 
