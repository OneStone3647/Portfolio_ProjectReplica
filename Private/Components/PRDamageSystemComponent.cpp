// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRDamageSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"

UPRDamageSystemComponent::UPRDamageSystemComponent()
{
	StatSystem = nullptr;
	// Health = 100.0f;
	// MaxHealth = 100.0f;
	// bIsInvincible = false;
	// bIsDead = false;
	bIsInterruptible = true;
	bIsBlocking = false;
}

void UPRDamageSystemComponent::OnRegister()
{
	Super::OnRegister();

	if(GetPROwner() != nullptr)
	{
		BindStatSystem(GetPROwner()->GetStatSystem());
	}
}

void UPRDamageSystemComponent::BindStatSystem(UPRStatSystemComponent* NewStatSystem)
{
	StatSystem = NewStatSystem;
}

float UPRDamageSystemComponent::Heal(float Amount)
{
	if(GetPROwner() != nullptr
		&& !GetPROwner()->IsDead()
		&& StatSystem != nullptr)
	{
		// Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
		//
		// return Health;
		
		FPRCharacterStat CharacterStat = StatSystem->GetCharacterStat();
		CharacterStat.HealthPoint = FMath::Clamp(CharacterStat.HealthPoint + Amount, 0.0f, CharacterStat.MaxHealthPoint);

		StatSystem->SetCharacterStat(CharacterStat);

		return CharacterStat.HealthPoint;
	}

	return 0.0f;
}

bool UPRDamageSystemComponent::TakeDamage(FPRDamageInfo DamageInfo)
{
	if(GetPROwner() == nullptr
		|| GetPROwner()->IsDead()
		|| StatSystem == nullptr)
	{
		return false;
	}
	
	EPRCanBeDamaged CanBeDamagedResult = CanBeDamaged(DamageInfo.bShouldDamageInvincible, DamageInfo.bCanBeBlocked);
	FPRCharacterStat CharacterStat = StatSystem->GetCharacterStat();
	
	switch(CanBeDamagedResult)
	{
	case EPRCanBeDamaged::CanBeDamaged_BlockDamage:
		if(OnBlockedDelegate.IsBound())
		{
			OnBlockedDelegate.Broadcast(DamageInfo.bCanBeParried);
		}
		
		return false;
	case EPRCanBeDamaged::CanBeDamaged_DoDamage:
		// Health -= DamageInfo.Amount;
		// if(Health <= 0.0f)
		// {
		// 	bIsDead = true;
		//
		// 	if(OnDeathDelegate.IsBound())
		// 	{
		// 		OnDeathDelegate.Broadcast();
		// 	}
		// }
		
		StatSystem->SetHealthPoint(CharacterStat.HealthPoint -= DamageInfo.Amount);
		if(CharacterStat.HealthPoint <= 0.0f)
		{
			// bIsDead = true;

			if(OnDeathDelegate.IsBound())
			{
				OnDeathDelegate.Broadcast();
			}
		}
		else
		{
			if(bIsInterruptible || DamageInfo.bShouldForceInterrupt)
			{
				if(OnDamageResponseDelegate.IsBound())
				{
					OnDamageResponseDelegate.Broadcast(DamageInfo.DamageResponse);
				}

				return true;
			}
		}
		
		return true;
	case EPRCanBeDamaged::CanBeDamaged_NoDamage:
		break;
	default:
		return false;
	}
	
	return false;
}

// bool UPRDamageSystemComponent::IsDead() const
// {
// 	return bIsDead;
// }

EPRCanBeDamaged UPRDamageSystemComponent::CanBeDamaged(const bool& bShouldDamageInvincible, const bool& bCanBeBlocked)
{
	if(GetPROwner() != nullptr
		&& !GetPROwner()->IsDead()
		&& (!GetPROwner()->IsInvincible() || bShouldDamageInvincible))
	{
		if(bIsBlocking && bCanBeBlocked)
		{
			return EPRCanBeDamaged::CanBeDamaged_BlockDamage;
		}
		else
		{
			return EPRCanBeDamaged::CanBeDamaged_DoDamage;
		}
	}

	return EPRCanBeDamaged::CanBeDamaged_NoDamage;
}

// float UPRDamageSystemComponent::GetHealth() const
// {
// 	return Health;
// }
//
// float UPRDamageSystemComponent::GetMaxHealth() const
// {
// 	return MaxHealth;
// }
