// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRKeyboardSkillPaletteSlotWidget.h"
#include "PRPlayerController.h"
#include "Components/Image.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

UPRKeyboardSkillPaletteSlotWidget::UPRKeyboardSkillPaletteSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputKeyIcon = nullptr;
	InputKeyIconImage = nullptr;
	ActionMappingName = TEXT("");
}

void UPRKeyboardSkillPaletteSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeInputKeyIcon();
}

void UPRKeyboardSkillPaletteSlotWidget::InitializeInputKeyIcon()
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(IsValid(PRPlayerController) == true && PRPlayerController->GetKeyboardInputKeyIconDataTable() != nullptr)
	{
		UDataTable* KeyboardInputKeyIconDataTable = PRPlayerController->GetKeyboardInputKeyIconDataTable();
		TArray<FInputActionKeyMapping> ActionMappings;
		UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionMappings);
		for(const FInputActionKeyMapping& ActionMapping : ActionMappings)
		{
			TArray<FName> RowNames = KeyboardInputKeyIconDataTable->GetRowNames();
			for(const FName& RowName : RowNames)
			{
				FPRInputKey DataTableRow = *KeyboardInputKeyIconDataTable->FindRow<FPRInputKey>(RowName, FString(""));
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

void UPRKeyboardSkillPaletteSlotWidget::SetInputKeyIconImage(UTexture2D* NewInputKeyIconImage)
{
	InputKeyIconImage = NewInputKeyIconImage;
}

FName UPRKeyboardSkillPaletteSlotWidget::GetActionMappingName() const
{
	return ActionMappingName;
}
