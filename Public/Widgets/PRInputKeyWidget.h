// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRInputKeyWidget.generated.h"

class UImage;

/**
* 입력키의 이미지를 바인딩된 입력키의 Pressed, Released 동작에 해당하는 위젯 애니메이션을 재생하는 UserWidget 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRInputKeyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRInputKeyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	/** 바인딩된 입력키가 눌러졌는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRInputKeyWidget")
	bool IsPressed() const;
	
	/** 바인딩된 입력키가 눌러졌을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRInputKeyWidget")
	void Pressed();
	virtual void Pressed_Implementation();

	/** 바인딩된 입력키가 떼어졌을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PRInputKeyWidget")
	void Released();
	virtual void Released_Implementation();

protected:
	/** 초기화하는 함수입니다. InputKeyMappingName에 해당하는 InputKey의 Icon을 DataTable에서 가져와 InputKeyIcon으로 설정합니다.*/
	UFUNCTION(BlueprintCallable, Category = "PRInputKeyWidget")
	void InitializeInputKey();

protected:
	/** 게임패드를 사용하는지 나타내는 변수입니다. 게임패드를 사용하지 않으면 InputKeyIcon을 키보드 아이콘으로 사용합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInputKeyWidget")
	bool bUseGamepad;
	
	/** 바인딩된 입력키의 아이콘입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRInputKeyWidget", meta = (BindWidget))
	UImage* InputKeyIcon;

	/** 입력키 아이콘의 이미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRInputKeyWidget", meta = (AllowPrivateAccess = "true"))
	UTexture2D* InputKeyIconImage;
	
	/** 바인딩할 입력키의 매핑의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInputKeyWidget")
	FName InputKeyMappingName;

	/** 입력 키의 설명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRInputKeyWidget")
	FText InputKeyDescription;

	/** 입력키가 눌러졌는지 나타내는 변수입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRInputKeyWidget")
	bool bIsPressed;

	/** 바인딩된 입력키가 눌러지는 것을 표현한 WidgetAnimation입니다. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "PRInputKeyWidget", meta = (AllowPrivateAccess = "true", BindWidgetAnim))
	UWidgetAnimation* PressedAnim;

	/** 바인딩된 입력키가 떼어지는 것을 표현한 WidgetAnimation입니다. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "PRInputKeyWidget", meta = (AllowPrivateAccess = "true", BindWidgetAnim))
	UWidgetAnimation* ReleasedAnim;
};
