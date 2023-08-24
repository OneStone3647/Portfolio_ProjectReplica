// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRInGameHUD.h"
#include "Widgets/PRHealthPointBarWidget.h"
#include "Widgets/PRComboCountWidget.h"
#include "Widgets/PRInteractWidget.h"
// #include "Widgets/PRGamepadSkillPaletteWidget.h"
// #include "Widgets/PRKeyboardSkillPaletteWidget.h"
#include "PRPlayerController.h"
#include "Widgets/PRBaseInGameInterfaceWidget.h"
#include "Characters/PRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UPRInGameHUD::UPRInGameHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// UI
	HealthPointBarWidget = nullptr;
	ComboCountWidget = nullptr;
	InteractWidget = nullptr;
	GamepadInGameInterfaceWidget = nullptr;
	KeyboardInGameInterfaceWidget = nullptr;
}

void UPRInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPRInGameHUD::SetIsGamepadInput(bool bNewIsGamepadInput)
{
	bIsGamepadInput = bNewIsGamepadInput;

	if(bIsGamepadInput)
	{
		if(GamepadInGameInterfaceWidget != nullptr)
		{
			GamepadInGameInterfaceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		
		if(KeyboardInGameInterfaceWidget != nullptr)
		{
			KeyboardInGameInterfaceWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if(GamepadInGameInterfaceWidget != nullptr)
		{
			GamepadInGameInterfaceWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		if(KeyboardInGameInterfaceWidget != nullptr)
		{
			KeyboardInGameInterfaceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UPRInGameHUD::InitializeWidgets_Implementation()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(IsValid(PRPlayerCharacter) == true)
	{
		if(HealthPointBarWidget != nullptr)
		{
			HealthPointBarWidget->BindStatSystem(PRPlayerCharacter->GetStatSystem());
		}

		if(ComboCountWidget != nullptr)
		{
			ComboCountWidget->BindOwnerPRPlayerCharacter(PRPlayerCharacter);
			ComboCountWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if(InteractWidget != nullptr)
		{
			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if(GamepadInGameInterfaceWidget != nullptr)
		{
			APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if(IsValid(PRPlayerController) == true)
			{
				GamepadInGameInterfaceWidget->BindOwnerPRPlayerController(PRPlayerController);
			}
			
			GamepadInGameInterfaceWidget->BindOwnerPRPlayerCharacter(PRPlayerCharacter);
			if(bIsGamepadInput)
			{
				GamepadInGameInterfaceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				GamepadInGameInterfaceWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if(KeyboardInGameInterfaceWidget != nullptr)
		{
			KeyboardInGameInterfaceWidget->BindOwnerPRPlayerCharacter(PRPlayerCharacter);
			if(bIsGamepadInput)
			{
				KeyboardInGameInterfaceWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				KeyboardInGameInterfaceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
	}
}

UPRComboCountWidget* UPRInGameHUD::GetComboCountWidget() const
{
	return ComboCountWidget;
}

UPRInteractWidget* UPRInGameHUD::GetInteractWidget() const
{
	return InteractWidget;
}
