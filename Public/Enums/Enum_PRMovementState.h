// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 캐릭터의 움직임 상태를 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRMovementState : uint8
{
	MovementState_None			UMETA(DisplayName = "None"),
	MovementState_Ground		UMETA(DisplayName = "Ground"),
	MovementState_InAir			UMETA(DisplayName = "InAir"),
	MovementState_Ragdoll		UMETA(DisplayName = "Ragdoll")
};
