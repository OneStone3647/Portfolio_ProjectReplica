// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 무기의 장착 위치를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRWeaponEquipPosition : uint8
{
	WeaponEquipPosition_Main		UMETA(DisplayName = "Main"),
	WeaponEquipPosition_Sub			UMETA(DisplayName = "Sub")
};
