// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRInteractWidget.generated.h"

class UVerticalBox;

/**
 * 상호작용하는 오브젝트들을 나타내는 UserWidget 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRInteractWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	/**
	 * 인자로 받은 오브젝트를 바탕으로 InteractSlot을 생성하고 InteractList에 추가하는 함수입니다.
	 *
	 * @param NewInteractableObject InteractSlot으로 생성할 오브젝트입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "InteractList")
	void AddToInteractList(AActor* NewInteractableObject);

	/**
	 * 인자로 받은 오브젝트를 바탕으로 생성된 InteractSlot를 InteractList에서 탐색하여 제거하는 함수입니다.
	 *
	 * @param NewInteractableObject InteractList에서 제거할 InteractSlot의 바탕이된 오브젝트입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "InteractList")
	void RemoveToInteractList(AActor* NewInteractableObject);
	
	/** 입력받은 인자로 SelectInteractIndex를 설정하고 해당 Slot에 하이라이트를 주는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractList")
	void SetSelectInteractIndex(int32 NewSelectInteractIndex);

private:
	/** 입력장치에 해당하는 상호작용 선택 아이콘을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractList")
	UTexture2D* GetSelectInteractInputIcon() const;

private:
	/** 상호작용 목록을 나타내는 VerticalBox입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractList", meta = (AllowPrivateAccess = "true", BindWidget))
	UVerticalBox* InteractList;

	/** InteractList에서 Slot으로 사용할 UserWidget의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractList", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> InteractListSlotClass;

	/** 키보드 마우스가 입력장치일 때 사용하는 상호작용 선택 아이콘입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UTexture2D* SelectInteractKeyboardMouseInputIcon;

	/** 게임패드가 입력장치일 때 사용하는 상호작용 선택 아이콘입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UTexture2D* SelectInteractGamepadInputIcon;
	
	/** 상호작용을 실행하기 위해 선택한 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	int32 SelectInteractIndex;
};
