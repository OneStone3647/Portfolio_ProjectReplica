// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/PREnumFunctionLibrary.h"

FString PREnumFunctionLibrary::GetEnumDisplayNameToString(const TCHAR* Enum, int32 EnumValue) const
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
	if(EnumPtr == nullptr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
}
