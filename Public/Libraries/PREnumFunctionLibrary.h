// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * Enum과 관련된 ㅎ마수를 구현한 Library 클래스입니다.
 */
class PROJECTREPLICA_API PREnumFunctionLibrary
{
public:
	/**
	 * Enum 인자와 같은 이름의 Enum을 찾고 EnumValue 인자에 해당하는 값을 찾아 해당 값의 DisplayName을 String으로 변환합니다.
	 * @Enum: 찾으려는 Enum의 이름입니다. TEXT("") 형식으로 입력합니다.
	 * @EnumValue: 찾으려는 Enum에서 불러올 값입니다.
	 * ex)  PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	 *		PR_LOG_SCREEN("Gait: %s", *EnumFunctionLibrary->GetEnumToString(TEXT("EPRGait"), static_cast<uint8>(Gait)));
	 */
	FString GetEnumDisplayNameToString(const TCHAR* Enum, int32 EnumValue = 0) const;
};
