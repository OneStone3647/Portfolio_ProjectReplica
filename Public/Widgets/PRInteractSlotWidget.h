// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRInteractSlotWidget.generated.h"

/**
 * 상호작용하는 오브젝트의 정보를 출력하는 UserWidget 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRInteractSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRInteractSlotWidget(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * 인자로 받은 상호작용 오브젝트의 정보로 Slot의 정보를 초기화하는 함수입니다.
	 *
	 * @param NewInteractableObject 상호작용 오브젝트입니다.
	 */
	void InitializeSlot(AActor* NewInteractableObject);

private:
	/** 상호작용 오브젝트의 아이콘을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractSlot")
	UTexture2D* GetInteractObjectIcon() const;
	
	/** 입력장치에 해당하는 상호작용 실행 아이콘을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractSlot")
	UTexture2D* GetActivateInteractInputIcon() const;

	/** 상호작용의 설명을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractSlot")
	FText GetInteractDescription() const;

	/** 상호작용 실행 아이콘의 Visibility를 반환하는 함수입니다.*/
	UFUNCTION(BlueprintCallable, Category = "InteractSlot")
	ESlateVisibility GetActivateInputIconVisibility() const;

private:
	/** 선택되었는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InteractSlot", meta = (AllowPrivateAccess = "true"))
	bool bIsSelect;
	
	/** 상호작용 오브젝트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InteractSlot", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> InteractableObject;

	/** 키보드 마우스가 입력장치일 때 사용하는 상호작용 실행 아이콘입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ActivateInteractKeyboardMouseInputIcon;

	/** 게임패드가 입력장치일 때 사용하는 상호작용 실행 아이콘입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ActivateInteractGamepadInputIcon;
	
public:
	/** 입력받은 인자로 bIsSelect를 설정하는 함수입니다. */
	void SetIsSelect(bool bNewIsSelect);
	
	/** InteractableObject를 반환하는 함수입니다. */
	class AActor* GetInteractableObject() const;
};
