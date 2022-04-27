// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStateSystemComponent.h"

UPRStateSystemComponent::UPRStateSystemComponent()
{
	// CharacterState
	bIsHit = false;
	bIsDead = false;
	
	// Move
	bCanMove = true;

	// Dodge
	bCanDodge = true;
	bOnDodge = false;

	// Attack
	bOnAttack = false;
}

bool UPRStateSystemComponent::IsHit() const
{
	return bIsHit;
}

void UPRStateSystemComponent::SetIsHit(bool bFlag)
{
	bIsHit = bFlag;
}

bool UPRStateSystemComponent::IsDead() const
{
	return bIsDead;
}

void UPRStateSystemComponent::SetIsDead(bool bFlag)
{
	bIsDead = bFlag;
}

#pragma region Move
bool UPRStateSystemComponent::IsCanMove() const
{
	return bCanMove;
}

void UPRStateSystemComponent::SetCanMove(bool bFlag)
{
	bCanMove = bFlag;
}
#pragma endregion 

#pragma region Dodge
bool UPRStateSystemComponent::IsCanDodge() const
{
	return bCanDodge;
}

bool UPRStateSystemComponent::IsOnDodge() const
{
	return bOnDodge;
}

void UPRStateSystemComponent::SetCanDodge(bool bFlag)
{
	bCanDodge = bFlag;
}

void UPRStateSystemComponent::SetOnDodge(bool bFlag)
{
	bOnDodge = bFlag;
}
#pragma endregion

#pragma region Attack
bool UPRStateSystemComponent::IsCanAttack() const
{
	return !bOnAttack && !(bIsHit || bIsDead || bOnDodge);
}

bool UPRStateSystemComponent::IsOnAttack() const
{
	return bOnAttack;
}

void UPRStateSystemComponent::SetOnAttack(bool bFlag)
{
	bOnAttack = bFlag;
}
#pragma endregion 
