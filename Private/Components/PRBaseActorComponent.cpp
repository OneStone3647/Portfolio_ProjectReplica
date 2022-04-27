// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRBaseActorComponent.h"
#include "Characters/PRBaseCharacter.h"

UPRBaseActorComponent::UPRBaseActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// PRCharacterReference
	PROwner = nullptr;
}


void UPRBaseActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializePROwner();
}

#pragma region PRCharacterReference
void UPRBaseActorComponent::InitializePROwner()
{
	APRBaseCharacter* PRBaseCharacter = Cast<APRBaseCharacter>(GetOwner());
	if(PRBaseCharacter)
	{
		PROwner = PRBaseCharacter;
	}
}
#pragma endregion 

