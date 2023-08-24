// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStatSystemComponent.h"

UPRStatSystemComponent::UPRStatSystemComponent()
{
	CharacterStat = FPRCharacterStat();
	Gender = EPRGender::Gender_None;
}

void UPRStatSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetHealthPoint(CharacterStat.MaxHealthPoint);
}

void UPRStatSystemComponent::TakeDamage(float NewDamage)
{
	SetHealthPoint(FMath::Clamp<float>(CharacterStat.HealthPoint - NewDamage, 0.0f, CharacterStat.MaxHealthPoint));

	if(CharacterStat.HealthPoint == 0.0f && OnHealthPointIsZeroDelegate.IsBound() == true)
	{
		OnHealthPointIsZeroDelegate.Broadcast();
	}
}

// float UPRStatSystemComponent::GetHealthPointRatio() const
// {
// 	return CharacterStat.MaxHealthPoint < KINDA_SMALL_NUMBER ? 0.0f : CharacterStat.HealthPoint / CharacterStat.MaxHealthPoint;
// }

void UPRStatSystemComponent::SetHealthPoint(float NewHealthPoint)
{
	if(NewHealthPoint > CharacterStat.MaxHealthPoint)
	{
		CharacterStat.HealthPoint = CharacterStat.MaxHealthPoint;
	}
	else if(NewHealthPoint < KINDA_SMALL_NUMBER)
	{
		CharacterStat.HealthPoint = 0.0f;
	}
	else
	{
		CharacterStat.HealthPoint = NewHealthPoint;
	}

	if(OnHealthPointIsChangedDelegate.IsBound() == true)
	{
		OnHealthPointIsChangedDelegate.Broadcast();
	}
}

FPRCharacterStat UPRStatSystemComponent::GetCharacterStat() const
{
	return CharacterStat;
}

EPRGender UPRStatSystemComponent::GetGender() const
{
	return Gender;
}
