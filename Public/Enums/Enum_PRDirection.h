// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 캐릭터의 이동 방향을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRDirection : uint8
{
	Direction_None				UMETA(DisplayName = "None"),
	Direction_Forward			UMETA(DisplayName = "Forward"),
	Direction_Backward			UMETA(DisplayName = "Backward"),
	Direction_Left				UMETA(DisplayName = "Left"),
	Direction_Right				UMETA(DisplayName = "Right"),
	Direction_ForwardLeft		UMETA(DisplayName = "ForwardLeft"),
	Direction_ForwardRight		UMETA(DisplayName = "ForwardRight"),
	Direction_BackwardLeft		UMETA(DisplayName = "BackwardLeft"),
	Direction_BackwardRight		UMETA(DisplayName = "BackwardRight")
};
