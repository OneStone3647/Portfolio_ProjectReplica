// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectReplicaGameInstance.h"

UProjectReplicaGameInstance::UProjectReplicaGameInstance()
{
	// AIInfoDataTable
	AIInfoDataTable = nullptr;
}

UDataTable* UProjectReplicaGameInstance::GetAIInfoDataTable() const
{
	return AIInfoDataTable;
}
