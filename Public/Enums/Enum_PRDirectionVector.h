// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 방향벡터를 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRDirectionVector : uint8
{
    DirectionVector_ForwardVector		UMETA(DisplayName = "ForwardVector"),
	DirectionVector_RightVector			UMETA(DisplayName = "RightVector")
};
