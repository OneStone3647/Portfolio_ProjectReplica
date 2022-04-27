// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 캐릭터의 회전 방식을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRRotationMode : uint8
{
	RotationMode_VelocityDirection		UMETA(DisplayName = "VelocityDirection"),
	RotationMode_LookingDirection		UMETA(DisplayName = "LookingDirection")
};
