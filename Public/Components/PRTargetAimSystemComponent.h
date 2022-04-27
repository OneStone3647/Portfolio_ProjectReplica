// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/SphereComponent.h"
#include "PRTargetAimSystemComponent.generated.h"

/**
 * Targeting TraceChannel에 블록되는 Collision과 Target으로 지정되어 LockOn 되었을 때 활성화하는 위젯을 가진 SphereComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRTargetAimSystemComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UPRTargetAimSystemComponent();

#pragma region LockOnWidget
protected:
	/** Target으로 지정되어 LockOn 되었을 때 활성화하는 위젯입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRTargetAimSystem|LockOnWidget")
	class UWidgetComponent* LockOnWidget;

public:
	/** LockOnWidget을 반환하는 함수입니다. */
	FORCEINLINE class UWidgetComponent* GetLockOnWidget() const
	{
		return LockOnWidget;
	}
#pragma endregion 
};
