// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

/**
 * 플레이어가 사용할 PlayerController 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
