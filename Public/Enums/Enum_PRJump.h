// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"

/**
 * 점프를 구분하는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRJump : uint8
{
	Jump_Start		UMETA(DisplayName = "JumpStart"),
	Jump_End		UMETA(DiplayNAme = "JumpEnd")
};
