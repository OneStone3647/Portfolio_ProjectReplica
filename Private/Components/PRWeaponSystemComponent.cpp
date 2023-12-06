// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRWeaponSystemComponent.h"

#include "Characters/PRBaseCharacter.h"
#include "Weapons/PRBaseWeapon.h"
#include "Weapons/PRDualMeleeWeapons.h"

UPRWeaponSystemComponent::UPRWeaponSystemComponent()
{
	WeaponInventory.Empty();
	EquippedWeaponIndex = 0;
	WeaponsToSpawn.Empty();
}

void UPRWeaponSystemComponent::InitializeWeaponInventory()
{
	WeaponInventory.Empty();

	// if(WeaponDataTable != nullptr)
	// {
	// 	TArray<FName> RowNames = WeaponDataTable->GetRowNames();
	//
	// 	for(const auto& RowName : RowNames)
	// 	{
	// 		TSubclassOf<APRBaseWeapon> NewWeaponClass = *WeaponDataTable->FindRow<TSubclassOf<APRBaseWeapon>>(RowName, FString(""));
	// 		APRBaseWeapon* NewWeapon = SpawnWeaponInWorld(NewWeaponClass);
	// 		if(IsValid(NewWeapon) == true)
	// 		{
	// 			NewWeapon->SetActivate(false);
	// 			WeaponInventory.Emplace(NewWeapon);
	// 		}
	// 	}
	// }

	if(WeaponsToSpawn.Num() != 0)
	{
		for(const auto& NewWeaponClass : WeaponsToSpawn)
		{
			APRBaseWeapon* NewWeapon = SpawnWeaponInWorld(NewWeaponClass);
			if(IsValid(NewWeapon) == true)
			{
				NewWeapon->InitializeWeapon(GetPROwner());
				NewWeapon->SetActivate(false);
				WeaponInventory.Emplace(NewWeapon);
			}
		}
	}
}

APRBaseWeapon* UPRWeaponSystemComponent::GetWeapon(int32 NewWeaponInventoryIndex) const
{
	if(WeaponInventory.IsValidIndex(NewWeaponInventoryIndex) == true)
	{
		return WeaponInventory[NewWeaponInventoryIndex];
	}

	return nullptr;
}

APRBaseWeapon* UPRWeaponSystemComponent::DrawWeapon(int32 DrawWeaponIndex)
{
	// WeaponInventory에 Index 값에 해당하는 무기가 존재하는지 확인합니다.
	if(WeaponInventory.IsValidIndex(DrawWeaponIndex) == true)
	{
		if(EquippedWeaponIndex != DrawWeaponIndex && WeaponInventory.IsValidIndex(EquippedWeaponIndex) == true)
		{
			WeaponInventory[EquippedWeaponIndex]->SetActivate(false);
		}

		EquippedWeaponIndex = DrawWeaponIndex;
		
		APRBaseWeapon* NewDrawWeapon = WeaponInventory[EquippedWeaponIndex];
		
		// 무기가 비활성화되었으면 활성화합니다.
		if(NewDrawWeapon->IsActivate() == false)
		{
			NewDrawWeapon->SetActivate(true);
		}

		NewDrawWeapon->Draw();

		return NewDrawWeapon;
	}

	return nullptr;
}

APRBaseWeapon* UPRWeaponSystemComponent::SheathWeapon(int32 SheathWeaponIndex)
{
	// WeaponInventory에 Index 값에 해당하는 무기가 존재하는지 확인합니다.
	if(WeaponInventory.IsValidIndex(SheathWeaponIndex) == true)
	{
		if(EquippedWeaponIndex != SheathWeaponIndex && WeaponInventory.IsValidIndex(EquippedWeaponIndex) == true)
		{
			WeaponInventory[EquippedWeaponIndex]->SetActivate(false);
		}

		EquippedWeaponIndex = SheathWeaponIndex;
		
		APRBaseWeapon* NewSheathWeapon = WeaponInventory[EquippedWeaponIndex];
		
		// 무기가 비활성화되었으면 활성화합니다.
		if(NewSheathWeapon->IsActivate() == false)
		{
			NewSheathWeapon->SetActivate(true);
		}

		NewSheathWeapon->Sheath();

		return NewSheathWeapon;
	}

	return nullptr;
}

APRBaseWeapon* UPRWeaponSystemComponent::GetEquippedWeapon() const
{
	if(WeaponInventory.IsValidIndex(EquippedWeaponIndex) == true)
	{
		return WeaponInventory[EquippedWeaponIndex];
	}

	return nullptr;
}

bool UPRWeaponSystemComponent::IsValidWeaponIndex(int32 NewWeaponIndex)
{
	return WeaponInventory.IsValidIndex(NewWeaponIndex);
}

APRBaseWeapon* UPRWeaponSystemComponent::SpawnWeaponInWorld(TSubclassOf<APRBaseWeapon> NewPRWeaponClass)
{
	return GetWorld()->SpawnActor<APRBaseWeapon>(NewPRWeaponClass);
}

TArray<APRBaseWeapon*> UPRWeaponSystemComponent::GetWeaponInventory() const
{
	return WeaponInventory;
}

int32 UPRWeaponSystemComponent::GetEquippedWeaponIndex() const
{
	return EquippedWeaponIndex;
}
