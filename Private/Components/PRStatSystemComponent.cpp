// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStatSystemComponent.h"

UPRStatSystemComponent::UPRStatSystemComponent()
{
	// InitializeComponent 함수를 사용하도록 설정합니다.
	bWantsInitializeComponent = true;

	// CharacterStat
	CharacterStat = FPRCharacterStat();
}

void UPRStatSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetHealthPoint(CharacterStat.MaxHealthPoint);
}

void UPRStatSystemComponent::TakeDamage(float Damage)
{
	SetHealthPoint(FMath::Clamp<float>(HealthPoint - Damage, 0.0f, CharacterStat.MaxHealthPoint));

	if(HealthPoint == 0.0f && OnHealthPointIsZero.IsBound() == true)
	{
		OnHealthPointIsZero.Broadcast();
	}
}

float UPRStatSystemComponent::GetHealthPointRatio() const
{
	return (CharacterStat.MaxHealthPoint < KINDA_SMALL_NUMBER) ? 0.0f : (HealthPoint / CharacterStat.MaxHealthPoint);
}

FPRCharacterStat UPRStatSystemComponent::GetCharacterStat() const
{
	return CharacterStat;
}

void UPRStatSystemComponent::SetHealthPoint(float NewHealthPoint)
{
	HealthPoint = NewHealthPoint;

	if(OnHealthPointIsChanged.IsBound() == true)
	{
		OnHealthPointIsChanged.Broadcast();
	}

	// float의 값을 0과 비교할 때는 미세한 오차 범위 내에 있는지를 KINDA_SMALL_NUMBER 키워드를 사용하여 판단하는 것이 좋습니다.
	if(HealthPoint < KINDA_SMALL_NUMBER)
	{
		HealthPoint = 0.0f;
	}
}
