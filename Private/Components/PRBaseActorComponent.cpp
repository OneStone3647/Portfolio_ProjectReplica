// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRBaseActorComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UPRBaseActorComponent::UPRBaseActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// PRCharacterReference
	PROwner = nullptr;
}

void UPRBaseActorComponent::OnRegister()
{
	Super::OnRegister();
	
	InitializePROwner();
}

void UPRBaseActorComponent::InitializePROwner()
{
	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(GetOwner());
	if(IsValid(NewPROwner) == true)
	{
		PROwner = NewPROwner;
	}
}

APRBaseCharacter* UPRBaseActorComponent::GetPROwner() const
{
	return PROwner;
}

