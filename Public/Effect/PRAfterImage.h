// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Objects/PRPooledObject.h"
#include "PRAfterImage.generated.h"

class UPoseableMeshComponent;

/**
 * 캐릭터의 잔상(AfterImage)을 표현하는 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRAfterImage : public APRPooledObject
{
	GENERATED_BODY()
	
public:	
	APRAfterImage();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	/**
	 * AfterImage가 활성화됐는지 판별하는 함수입니다.
	 * @return AfterImage가 활성화됐을 경우 true를 아닐 경우 false를 반환합니다.
	 */
	virtual bool IsActivate() const override;

private:
	/** AfterImageMaterial로 PoseableMesh가 사용할 UMaterialInstanceDynamic를 생성하는 함수입니다. */
	void CreatePoseableMeshMaterials(USkeletalMeshComponent* NewMesh);
	
	/**
	 * AfterImage가 실행될 때 시간이 지남에 따라 AfterImageMaterial의 Opacity 값을 서서히 줄여서
	 * 0까지 설정하여 사라지는 효과를 주는 함수입니다.
	 */
	void FadeOut(float DeltaTime);
	
	/** AfterImage의 실행이 끝난 후 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRGhostTrail")
	void Disable();

	/**
	 * DynamicMaterials의 Opacity값을 조절하는 함수입니다.
	 * @param Value Curve에 의해 받는 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRGhostTrail")
	void SetOpacity(float Value);
	
private:
	/** 캐릭터의 Mesh의 포즈를 복사할 PoseableMeshComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	UPoseableMeshComponent* PoseableMesh;

	/** GhostTrailMaterial을 바탕으로 생성된 MaterialInstanceDynamic의 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	/** PoseableMesh에 적용할 잔상 Material입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* AfterImageMaterial;

	/** AfterImageMaterial의 색입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	FLinearColor AfterImageColor;

	/** AfterImageMaterial의 Opacity 값이 0이 될 때 까지의 시간입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRGhostTrail", meta = (AllowPrivateAccess = "true"))
	float FadeOutTime;

	/** AfterImageMaterial의 Opacity 값이 0이 될 때 까지의 카운트 다운 시간입니다. */
	float FadeOutCountDown;

#pragma region PooledObject
public:
	/** 오브젝트를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;
	
	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;
#pragma endregion 
};
