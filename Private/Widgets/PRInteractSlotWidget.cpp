// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRInteractSlotWidget.h"
#include "PRPlayerController.h"
#include "Interfaces/Interface_PRInteract.h"
#include "Interfaces/PRInteractInterface.h"
#include "Kismet/GameplayStatics.h"

UPRInteractSlotWidget::UPRInteractSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsSelect = false;
	InteractableObject = nullptr;
}

void UPRInteractSlotWidget::InitializeSlot(AActor* NewInteractableObject)
{
	InteractableObject = NewInteractableObject;
}

UTexture2D* UPRInteractSlotWidget::GetInteractObjectIcon() const
{
	if(InteractableObject->GetClass()->ImplementsInterface(UInterface_PRInteract::StaticClass()) == true)
	{
		return IInterface_PRInteract::Execute_GetInteractInfo(InteractableObject.Get()).Icon;
	}

	return nullptr;
}

UTexture2D* UPRInteractSlotWidget::GetActivateInteractInputIcon() const
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PRPlayerController != nullptr)
	{
		if(PRPlayerController->IsGamepadInput() == true)
		{
			return ActivateInteractGamepadInputIcon;
		}
	}

	return ActivateInteractKeyboardMouseInputIcon;
}

FText UPRInteractSlotWidget::GetInteractDescription() const
{
	if(InteractableObject->GetClass()->ImplementsInterface(UInterface_PRInteract::StaticClass()) == true)
	{
		return FText::FromString(IInterface_PRInteract::Execute_GetInteractInfo(InteractableObject.Get()).Description);
	}

	return FText();
}

ESlateVisibility UPRInteractSlotWidget::GetActivateInputIconVisibility() const
{
	if(bIsSelect)
	{
		return ESlateVisibility::HitTestInvisible;
	}

	return ESlateVisibility::Hidden;
}

void UPRInteractSlotWidget::SetIsSelect(bool bNewIsSelect)
{
	bIsSelect = bNewIsSelect;
}

AActor* UPRInteractSlotWidget::GetInteractableObject() const
{
	return InteractableObject.Get();
}
