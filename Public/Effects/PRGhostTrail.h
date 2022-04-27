// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "PRGhostTrail.generated.h"

// DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityIsZeroDelegate, class APRGhostTrail*)

/**
 * 캐릭터의 잔상(GhostTrail)을 표현하는 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRGhostTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	APRGhostTrail();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	/**
	 * 인자로 받은 캐릭터의 Mesh로 GhostTrail을 실행하는 함수입니다.
	 * @param PROwner PoseableMesh에 적용할 캐릭터입니다.
	 * @param Transform 적용할 Transform입니다. 
	 */
	void Execute(class APRBaseCharacter* PROwner, FTransform Transform);

private:
	/** GhostTrail이 실행이 끝난 후 비활성화하는 함수입니다. */
	UFUNCTION()
	void Disable();
	
private:
	/** 캐릭터 Mesh의 포즈를 복사할 PoseableMeshComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	UPoseableMeshComponent* PoseableMesh;

	/** PoseableMeshComponent에 적용할 반투명 Material입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* FadeMaterial;

	/** FadeMaterial을 바탕으로 생성된 MaterialInstanceDynamic의 배열입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	/** GhostTrail의 색입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	FLinearColor GhostTrailColor;

#pragma region Opacity
private:
	/**
	 * OpacityTimeline을 초기화하는 함수입니다.
	 * OpacityTimeline가 사용하는 Callback 함수에 사용할 함수를 바인드합니다.
	 * OpacityTimeline에 Curve를 추가합니다.
	 */
	void InitializeOpacityTimeline();

	/**
	 * FadeMaterial의 Opacity 값을 조절하는 함수입니다.
	 * @param Value Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void SetOpacity(float Value);
	
private:
	/** FadeMaterial의 Opacity 값을 조절할 때 사용하는 Timeline입니다. */
	FTimeline OpacityTimeline;
	
	/** FadeMaterial의 Opacity값의 조절에 사용하는 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRGhostTrail|Opacity", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* OpacityCurveFloat;
	
// public:
// 	/** Opacity가 0일 경우 실행하는 Delegate입니다. */
// 	FOnOpacityIsZeroDelegate OnOpacityIsZeroDelegate;
#pragma endregion
};
