// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRSkillSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Skills/PRBaseSkill.h"

UPRSkillSystemComponent::UPRSkillSystemComponent()
{
	// ConstructSkillInventory.Empty();
	SkillInventory.Empty();

	// DataTable
	ConstructSkillInfoDataTable = nullptr;
}

void UPRSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeSkillInventory();
}

void UPRSkillSystemComponent::InitializeSkillInventory()
{
	SkillInventory.Empty();
	// if(ConstructSkillInventory.Num() > 0)
	// {
	// 	for(TSubclassOf<UPRBaseSkill> ConstructSkill : ConstructSkillInventory)
	// 	{
	// 		UPRBaseSkill* NewConstructSkill = ConstructSkillInWorld(ConstructSkill);
	// 		if(NewConstructSkill != nullptr)
	// 		{
	// 			AddToSkillInventory(NewConstructSkill);
	// 		}
	// 	}
	// }

	if(ConstructSkillInfoDataTable != nullptr)
	{
		SkillInventory = CreateSkillFromDataTable(ConstructSkillInfoDataTable);
	}
}

void UPRSkillSystemComponent::AddToSkillInventory(UPRBaseSkill* NewSkill)
{
	SkillInventory.Emplace(NewSkill);
}

UPRBaseSkill* UPRSkillSystemComponent::GetSkillFromCommand(EPRCommandSkill NewCommandSkill) const
{
	if(SkillInventory.Num() > 0)
	{
		for(UPRBaseSkill* Skill : SkillInventory)
		{
			if(Skill->GetSkillInfo().CommandSkill == NewCommandSkill)
			{
				return Skill;				
			}
		}
	}

	return nullptr;
}

UPRBaseSkill* UPRSkillSystemComponent::GetSkillFromName(const FText& NewSkillName) const
{
	if(SkillInventory.Num() > 0)
	{
		for(UPRBaseSkill* Skill : SkillInventory)
		{
			if(Skill->GetSkillInfo().Name.EqualTo(NewSkillName) == true)
			{
				return Skill;
			}
		}
	}

	return nullptr;
}

UPRBaseSkill* UPRSkillSystemComponent::CreateSkill(TSubclassOf<UPRBaseSkill> NewPRSkillClass)
{
	if(NewPRSkillClass != nullptr)
	{
		UPRBaseSkill* NewConstructSkill = NewObject<UPRBaseSkill>(GetOwner(), NewPRSkillClass);
		if(IsValid(NewConstructSkill) == true)
		{
			NewConstructSkill->SetSkillOwner(GetPROwner());
		
			return NewConstructSkill;
		}
	}

	return nullptr;
}

TArray<UPRBaseSkill*> UPRSkillSystemComponent::CreateSkillFromDataTable(UDataTable* NewSkillDataTable)
{
	TArray<UPRBaseSkill*> NewConstructSkills;
	if(NewSkillDataTable != nullptr)
	{
		TArray<FName> RowNames = NewSkillDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRConstructSkillInfo* DataTableRow = NewSkillDataTable->FindRow<FPRConstructSkillInfo>(RowName, FString(""));
			if(DataTableRow != nullptr)
			{
				UPRBaseSkill* NewConstructSkill = CreateSkill(DataTableRow->ConstructSkillClass);
				if(IsValid(NewConstructSkill) == true)
				{
					NewConstructSkill->SetSkillInfo(DataTableRow->SkillInfo);
					NewConstructSkill->InitializeSkill();
					NewConstructSkills.Emplace(NewConstructSkill);
				}
			}
		}
	}

	return NewConstructSkills;
}
