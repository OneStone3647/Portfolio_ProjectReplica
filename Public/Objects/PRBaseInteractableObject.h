// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PRInteractInterface.h"
#include "PRBaseInteractableObject.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTREPLICA_API APRBaseInteractableObject : public AActor, public IPRInteractInterface
{
	GENERATED_BODY()
	
public:	
	APRBaseInteractableObject();

protected:
	virtual void BeginPlay() override;
	
public:
	/**
	 * 상호작용을 실행하는 함수입니다.
	 *
	 * @return 상호작용의 실행되었을 경우 true를 반환합니다. 실행하지 않을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	bool OnInteract();
	virtual bool OnInteract_Implementation() override;

	/** InteractInfo를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	FPRInteractInfo GetInteractInfo();
	virtual FPRInteractInfo GetInteractInfo_Implementation() override;	

protected:
	/** BoxComponent의 OnComponentBeginOverlap Delegate에 바인딩하여 Overlap되었을 때 실행되는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractOverlap")
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** BoxComponent의 OnComponentEndOverlap Delegate에 바인딩하여 Overlap이 끝났을 때 실행되는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InteractOverlap")
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	/** 상호작용에 대한 정보입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractInfo")
	FPRInteractInfo InteractInfo;

private:
	/** Overlap에 사용하는 BoxComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InteractOverlap", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

public:
	/** BoxComponent를 반환하는 함수입니다. */
	FORCEINLINE class UBoxComponent* GetBoxComponent() const
	{
		return BoxComponent;
	}
};
