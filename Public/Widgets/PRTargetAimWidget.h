// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRTargetAimWidget.generated.h"

/**
 * 플레이어 캐릭터가 이 위젯을 가지고 있는 캐릭터를 Target으로 LockOn 했을 경우 이미지를 활성화하는 위젯 클래스입니다.
 * 플레이어 캐릭터가 Targeting을 할 때 이 위젯을 가지고 있으면 Target으로 지정이 가능합니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRTargetAimWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRTargetAimWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;
	
private:
	/** Target으로 LockOn 했을 경우 활성화하는 이미지입니다. */
	UPROPERTY(meta = (AllowPrivateAccess = "true", BindWidget))
	class UImage* LockOnIndicatorImage;
};
