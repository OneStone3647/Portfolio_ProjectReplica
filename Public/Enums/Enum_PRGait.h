// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 캐릭터의 걷는 모양을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRGait : uint8
{
	Gait_Walking		UMETA(DisplayName = "Walking"),
	Gait_Running		UMETA(DisplayName = "Running"),
	Gait_Sprinting		UMETA(DisplayName = "Sprinting")
};
