// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "UObject/Interface.h"
#include "PRInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPRInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 상호작용을 실행하는 Interface입니다.
 */
class PROJECTREPLICA_API IPRInteractInterface
{
	GENERATED_BODY()

public:
	/**
	 * 상호작용을 실행하는 함수입니다.
	 *
	 * @return 상호작용의 실행되었을 경우 true를 반환합니다. 실행하지 않을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	bool OnInteract();

	/** InteractInfo를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	FPRInteractInfo GetInteractInfo();
};
