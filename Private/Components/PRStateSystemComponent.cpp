// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStateSystemComponent.h"

#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UPRStateSystemComponent::UPRStateSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Debug
	bActivateDebug = false;

	// TakeDamage
	bIsHit = false;
	bIsDead = false;
	bIsInvincible = false;
	
	// CharacterState
	bCanCancelAction = false;
	InitializeActionState();
	// bMoveable = true;
	// bDodgeable = true;
	// bAttackable = false;
	// bDodgeAttackable = false;
}

void UPRStateSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateDebug();
}

#pragma region Debug
void UPRStateSystemComponent::UpdateDebug()
{
	if(bActivateDebug)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "DodgeAttackable: " + UKismetStringLibrary::Conv_BoolToString(bDodgeAttackable), true, false, FLinearColor::Red, 0.0f);
		// UKismetSystemLibrary::PrintString(GetWorld(), "Attackable: " + UKismetStringLibrary::Conv_BoolToString(bAttackable), true, false, FLinearColor::Red, 0.0f);
		// UKismetSystemLibrary::PrintString(GetWorld(), "Dodgeable: " + UKismetStringLibrary::Conv_BoolToString(bDodgeable), true, false, FLinearColor::Red, 0.0f);
		// UKismetSystemLibrary::PrintString(GetWorld(), "Moveable: " + UKismetStringLibrary::Conv_BoolToString(bMoveable), true, false, FLinearColor::Red, 0.0f);
		UKismetSystemLibrary::PrintString(GetWorld(), "==StateSystem Debug==", true, false, FLinearColor::Green, 0.0f);
	}
}
#pragma endregion 

#pragma region TakeDamage
bool UPRStateSystemComponent::IsHit() const
{
	return bIsHit;
}

bool UPRStateSystemComponent::IsDead() const
{
	return bIsDead;
}

bool UPRStateSystemComponent::IsInvincible() const
{
	return bIsInvincible;
}

void UPRStateSystemComponent::SetIsHit(bool bNewIsHit)
{
	bIsHit = bNewIsHit;
}

void UPRStateSystemComponent::SetIsDead(bool bNewIsDead)
{
	bIsDead = bNewIsDead;
}

void UPRStateSystemComponent::SetIsInvincible(bool bNewIsInvincible)
{
	bIsInvincible = bNewIsInvincible;
}
#pragma endregion

bool UPRStateSystemComponent::IsCanCancelAction() const
{
	return bCanCancelAction;
}

void UPRStateSystemComponent::InitializeActionState()
{
	ActionState.Emplace(EPRAction::Action_Move, true);
	ActionState.Emplace(EPRAction::Action_Dodge, true);
	ActionState.Emplace(EPRAction::Action_DoubleJump, true);
	ActionState.Emplace(EPRAction::Action_NormalAttack, false);
	ActionState.Emplace(EPRAction::Action_FirstBattleSkill, false);
	ActionState.Emplace(EPRAction::Action_SecondBattleSkill, false);
	ActionState.Emplace(EPRAction::Action_ThirdBattleSkill, false);
	ActionState.Emplace(EPRAction::Action_Ultimate, false);
	ActionState.Emplace(EPRAction::Action_DodgeAttack, false);
}

bool UPRStateSystemComponent::IsActionable(EPRAction Action) const
{
	if(ActionState.Find(Action) != nullptr)
	{
		return *ActionState.Find(Action);
	}
	
	return false;
}

void UPRStateSystemComponent::SetActionable(EPRAction Action, bool bAble)
{
	ActionState.Emplace(Action, bAble);
}

void UPRStateSystemComponent::SetActionables(TMap<EPRAction, bool>& Actions)
{
	for(const auto& Action : Actions)
	{
		SetActionable(Action.Key, Action.Value);
	}
}

void UPRStateSystemComponent::SetDodgealbeAfterDelay(bool bNewDodgeable, float NewDelayTime)
{
	if(NewDelayTime <= 0.0f)
	{
		// SetDodgeable(bNewDodgeable);
		SetActionable(EPRAction::Action_Dodge, bNewDodgeable);
	}
	else
	{
		// FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UPRStateSystemComponent::SetDodgeable, bNewDodgeable);
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UPRStateSystemComponent::SetActionable, EPRAction::Action_Dodge, bNewDodgeable);
		GetWorld()->GetTimerManager().SetTimer(DodgeableDelayTimerHandle, TimerDelegate, NewDelayTime, false);
	}
}

void UPRStateSystemComponent::SetCanCancelAction(bool bNewCanCancelAction)
{
	bCanCancelAction = bNewCanCancelAction;
}
