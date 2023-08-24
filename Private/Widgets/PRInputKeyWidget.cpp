// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRInputKeyWidget.h"
#include "PRPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

UPRInputKeyWidget::UPRInputKeyWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseGamepad = true;
	InputKeyIcon = nullptr;
	InputKeyIconImage = nullptr;
	InputKeyMappingName = TEXT("MappingName");
	InputKeyDescription = FText();
	bIsPressed = false;
	PressedAnim = nullptr;
	ReleasedAnim = nullptr;
}

void UPRInputKeyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(InputKeyIconImage != nullptr && InputKeyIcon != nullptr)
	{
		InputKeyIcon->SetBrushFromTexture(InputKeyIconImage);
	}
}

void UPRInputKeyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeInputKey();
}

bool UPRInputKeyWidget::IsPressed() const
{
	return bIsPressed;
}

void UPRInputKeyWidget::Pressed_Implementation()
{
	bIsPressed = true;
	if(PressedAnim != nullptr)
	{
		PlayAnimation(PressedAnim);
	}
}

void UPRInputKeyWidget::Released_Implementation()
{
	bIsPressed = false;
	if(ReleasedAnim != nullptr)
	{
		PlayAnimation(ReleasedAnim);
	}
}

void UPRInputKeyWidget::InitializeInputKey()
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(IsValid(PRPlayerController) == true)
	{
		UDataTable* InputKeyIconDataTable = nullptr;
		// 입력장치에 해당하는 InputKey의 Icon이 보관된 DataTable을 가져옵니다.
		if(bUseGamepad)
		{
			InputKeyIconDataTable = PRPlayerController->GetGamepadInputKeyIconDataTable();
		}
		else
		{
			InputKeyIconDataTable = PRPlayerController->GetKeyboardInputKeyIconDataTable();
		}

		// InputKeyMapping에 해당하는 Key 값을 DataTable에서 찾아 Icon 이미지를 찾습니다.
		TArray<FInputActionKeyMapping> ActionMappings;
		UInputSettings::GetInputSettings()->GetActionMappingByName(InputKeyMappingName, ActionMappings);
		for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
		{
			TArray<FName> RowNames = InputKeyIconDataTable->GetRowNames();
			for(const FName& RowName : RowNames)
			{
				FPRInputKey DataTableRow = *InputKeyIconDataTable->FindRow<FPRInputKey>(RowName, FString(""));
				if(DataTableRow.Key == ActionMapping.Key)
				{
					InputKeyIconImage = DataTableRow.Icon;
					break;
				}
			}
		}
	}

	InputKeyIcon->SetBrushFromTexture(InputKeyIconImage);
}
