// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRBaseInGameInterfaceWidget.generated.h"

class APRPlayerCharacter;
class APRPlayerController;

/**
 * 인게임에서 사용하는 인터페이스 위젯을 모은 UserWidget 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRBaseInGameInterfaceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRBaseInGameInterfaceWidget(const FObjectInitializer& ObjectInitializer);

public:
	/** 인터페이스 위젯에 PRPlayerCharacter를 바인딩하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRBaseInGameInterface")
	void BindOwnerPRPlayerCharacter(const class APRPlayerCharacter* NewPRPlayerCharacter);
	virtual void BindOwnerPRPlayerCharacter_Implementation(const class APRPlayerCharacter* NewPRPlayerCharacter);

	/** 인터페이스 위젯에 PRPlayerController를 바인딩하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRBaseInGameInterface")
	void BindOwnerPRPlayerController(const class APRPlayerController* NewPRPlayerController);
	virtual void BindOwnerPRPlayerController_Implementation(const class APRPlayerController* NewPRPlayerController);
};
