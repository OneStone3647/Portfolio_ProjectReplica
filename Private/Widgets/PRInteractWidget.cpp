// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRInteractWidget.h"
#include "PRPlayerController.h"
#include "Components/VerticalBox.h"
#include "Interfaces/Interface_PRInteract.h"
#include "Widgets/PRInteractSlotWidget.h"
#include "Interfaces/PRInteractInterface.h"
#include "Kismet/GameplayStatics.h"

UPRInteractWidget::UPRInteractWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractList = nullptr;
	InteractListSlotClass = nullptr;
	SelectInteractKeyboardMouseInputIcon = nullptr;
	SelectInteractGamepadInputIcon = nullptr;
	SelectInteractIndex = 0;
}

void UPRInteractWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InteractList = Cast<UVerticalBox>(GetWidgetFromName(TEXT("InteractList")));
}

void UPRInteractWidget::AddToInteractList(AActor* NewInteractableObject)
{
	if(InteractListSlotClass != nullptr && NewInteractableObject->GetClass()->ImplementsInterface(UInterface_PRInteract::StaticClass()) == true)
	{
		UPRInteractSlotWidget* NewInteractSlot = Cast<UPRInteractSlotWidget>(CreateWidget(this, InteractListSlotClass));
		if(NewInteractSlot != nullptr)
		{
			NewInteractSlot->InitializeSlot(NewInteractableObject);
			InteractList->AddChildToVerticalBox(NewInteractSlot);
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UPRInteractWidget::RemoveToInteractList(AActor* NewInteractableObject)
{
	for(UWidget* InteractSlot : InteractList->GetAllChildren())
	{
		UPRInteractSlotWidget* NewInteractSlot = Cast<UPRInteractSlotWidget>(InteractSlot);
		if(NewInteractSlot != nullptr)
		{
			if(NewInteractSlot->GetInteractableObject() == NewInteractableObject)
			{
				InteractList->RemoveChild(NewInteractSlot);
				break;
			}
		}
	}

	if(InteractList->GetAllChildren().Num() == 0)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPRInteractWidget::SetSelectInteractIndex(int32 NewSelectInteractIndex)
{
	TArray<UWidget*> InteractSlots = InteractList->GetAllChildren();
	if(NewSelectInteractIndex >= 0 && NewSelectInteractIndex < InteractSlots.Num())
	{
		// 이전 Slot의 하이라이트를 해제합니다.
		UPRInteractSlotWidget* InteractSlot = Cast<UPRInteractSlotWidget>(InteractSlots[SelectInteractIndex]);
		if(InteractSlot != nullptr)
		{
			InteractSlot->SetIsSelect(false);
		}
		
		// 새로운 Slot의 하이라이트를 적용합니다.
		UPRInteractSlotWidget* NewInteractSlot = Cast<UPRInteractSlotWidget>(InteractSlots[NewSelectInteractIndex]);
		if(NewInteractSlot != nullptr)
		{
			NewInteractSlot->SetIsSelect(true);
		}
		
		SelectInteractIndex = NewSelectInteractIndex;
	}
}

UTexture2D* UPRInteractWidget::GetSelectInteractInputIcon() const
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PRPlayerController != nullptr)
	{
		if(PRPlayerController->IsGamepadInput() == true)
		{
			return SelectInteractGamepadInputIcon;
		}
	}

	return SelectInteractKeyboardMouseInputIcon;
}
