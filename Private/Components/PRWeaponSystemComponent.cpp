// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRWeaponSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Weapons/PRBaseWaepon.h"
#include "Enums/Enum_PRWeaponEquipPosition.h"

UPRWeaponSystemComponent::UPRWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// PRWeaponSystem
	WeaponIndex = 0;
	MainWeaponEquipSocketName = TEXT("hand_r_weapon");
	SubWeaponEquipSocketName = TEXT("hand_l_weapon");
	bCanSwapWeapon = true;
}

#pragma region WeaponInventory
void UPRWeaponSystemComponent::SpawnWeaponAndAddToInventory(TSubclassOf<APRBaseWaepon> NewWeapon)
{
	int32 NewWeaponIndex = 0;
	if(MainWeaponInventory.Num() != 0)
	{
		TArray<int32> Keys;
		MainWeaponInventory.GetKeys(Keys);

		NewWeaponIndex = Keys.Last() + 1;
	}

	APRBaseWaepon* NewSpawnMainWeapon = SpawnWeaponInWorld(NewWeapon);
	if(IsValid(NewSpawnMainWeapon) == true)
	{
		NewSpawnMainWeapon->SetPROnwerAndOwnerType(PROwner);
		MainWeaponInventory.Add(NewWeaponIndex, NewSpawnMainWeapon);

		if(NewSpawnMainWeapon->IsEqualWeaponEquipType(EPRWeaponEquipType::WeaponEquipType_Dual) == true)
		{
			APRBaseWaepon* NewSpawnSubWeapon = SpawnWeaponInWorld(NewWeapon);
			if(IsValid(NewSpawnSubWeapon) == true)
			{
				NewSpawnSubWeapon->SetPROnwerAndOwnerType(PROwner);
				SubWeaponInventory.Add(NewWeaponIndex, NewSpawnSubWeapon);
			}
		}
	}
}

void UPRWeaponSystemComponent::EquipWeapons(int32 NewWeaponIndex)
{
	if(MainWeaponInventory.Find(NewWeaponIndex) != nullptr)
	{
		MainWeapon = *MainWeaponInventory.Find(NewWeaponIndex);
	}
	else
	{
		MainWeapon = nullptr;
	}

	if(SubWeaponInventory.Find(NewWeaponIndex) != nullptr)
	{
		SubWeapon = *SubWeaponInventory.Find(NewWeaponIndex);
	}
	else
	{
		SubWeapon = nullptr;
	}
}
#pragma endregion 

void UPRWeaponSystemComponent::DrawWeapons()
{
	MainWeapon->InitializeWeaponPRAnimMontageIndex();
	
	if(MainWeapon)
	{
		DrawWeapon(MainWeapon, MainWeaponEquipSocketName, true);
	}

	if(SubWeapon)
	{
		DrawWeapon(SubWeapon, SubWeaponEquipSocketName, true);
	}
}

void UPRWeaponSystemComponent::SheathWeapons(bool bExecuteEffect)
{
	MainWeapon->InitializeWeaponPRAnimMontageIndex();

	if(MainWeapon)
	{
		SheathWeapon(MainWeapon, bExecuteEffect);
	}

	if(SubWeapon)
	{
		SheathWeapon(SubWeapon, bExecuteEffect);
	}
}

bool UPRWeaponSystemComponent::IsDrawWeapons() const
{
	bool MainWeaponCheck = IsValid(MainWeapon) == true && MainWeapon->IsWeaponActive() == true;
	bool SubWeaponCheck = IsValid(SubWeapon) == true && SubWeapon->IsWeaponActive() == true;

	return MainWeaponCheck || SubWeaponCheck;
}

void UPRWeaponSystemComponent::SwapWeapons()
{
	int32 LastWeaponIndex = 0;
	if(MainWeaponInventory.Num() != 0)
	{
		TArray<int32> Keys;
		MainWeaponInventory.GetKeys(Keys);

		LastWeaponIndex = Keys.Last();
	}

	// 마지막 Index일 경우 0(처음)으로 설정합니다.
	if(WeaponIndex == LastWeaponIndex)
	{
		WeaponIndex = 0;
	}
	else
	{
		WeaponIndex++;
	}

	if(IsDrawWeapons() == true)
	{
		SheathWeapons(true);
	}

	MainWeapon = nullptr;
	SubWeapon = nullptr;

	EquipWeapons(WeaponIndex);
}

bool UPRWeaponSystemComponent::IsCanSwapWeapon()
{
	return bCanSwapWeapon
			&& !(PROwner->GetStateSystem()->IsDead() == true
				|| PROwner->GetStateSystem()->IsHit() == true);
}

APRBaseWaepon* UPRWeaponSystemComponent::GetEquipWeapon(EPRWeaponEquipPosition WeaponEquipPosition) const
{
	switch(WeaponEquipPosition)
	{
	case EPRWeaponEquipPosition::WeaponEquipPosition_Main:
		return MainWeapon;
	case EPRWeaponEquipPosition::WeaponEquipPosition_Sub:
		return SubWeapon;
	default:
		return nullptr;			
	}
}

bool UPRWeaponSystemComponent::IsEquipWeapon(EPRWeaponEquipPosition WeaponEquipPosition) const
{
	switch(WeaponEquipPosition)
	{
	case EPRWeaponEquipPosition::WeaponEquipPosition_Main:
		if(IsValid(MainWeapon) == true)
		{
			return true;
		}
		return false;
	case EPRWeaponEquipPosition::WeaponEquipPosition_Sub:
		if(IsValid(SubWeapon) == true)
		{
			return true;
		}
		return false;
	default:
		return false;
	}
}


APRBaseWaepon* UPRWeaponSystemComponent::SpawnWeaponInWorld(TSubclassOf<APRBaseWaepon> NewWeapon)
{
	return GetWorld()->SpawnActor<APRBaseWaepon>(NewWeapon);
}

void UPRWeaponSystemComponent::DrawWeapon(APRBaseWaepon* Weapon, FName EquipSocketName, bool bExecuteEffect)
{
	if(IsValid(Weapon) == true)
	{
		Weapon->AttachToComponent(PROwner->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), EquipSocketName);
		Weapon->SetWeaponActive(true, bExecuteEffect);
	}
}

void UPRWeaponSystemComponent::SheathWeapon(APRBaseWaepon* Weapon, bool bExecuteEffect)
{
	if(IsValid(Weapon) == true)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->SetWeaponActive(false, bExecuteEffect);
	}
}

void UPRWeaponSystemComponent::SetCanSwapWeapon(bool bFlag)
{
	bCanSwapWeapon = bFlag;
}
