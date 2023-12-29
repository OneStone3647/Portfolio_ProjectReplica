// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Effect/PREffect.h"
#include "PRParticleEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParticleEffectDeactivate, UPRParticleEffect*, ParticleEffect);

/**
 * EffectSystem이 관리하는 ParticleEffect 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRParticleEffect : public UPREffect
{
	GENERATED_BODY()

public:
	UPRParticleEffect();

public:
	/** Effect를 초가화하는 함수입니다. */
	virtual void Initialize() override;

	/** Effect를 최신화하는 함수입니다. */
	// virtual void UpdateEffect(float DeltaTime) override;
	
	/** Effect의 활성화를 판별하는 함수입니다. */
	virtual bool IsActivate() const override;

	/** Effect를 활성화하는 함수입니다. */
	virtual void Activate() override;

	/** Effect를 비활성화하는 함수입니다. */
	virtual void Deactivate() override;
	
	/** Effect를 제거하는 함수입니다. */
	virtual void Destroy() override;
	
	/** Effect를 지정한 위치에 Spawn하는 함수입니다. */
	virtual void SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate) override;

	/** Effect를 지정한 Component에 부작하여 Spawn하는 함수입니다. */
	virtual void SpawnEffectAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate) override;
	
	/** Effect의 위치를 반환하는 함수입니다. */
	virtual FVector GetEffectLocation() const override;

	/** FXSystemComponent을 반환하는 함수입니다. */
	virtual UFXSystemComponent* GetFXSystemComponent() const override;
	
	/** Effect가 반복되는 경우 true를 반환하는 함수입니다. */
	virtual bool IsLooping() const override;
	
	/** TimeStop에 영향을 받을 때 실행하는 함수입니다. */
	virtual void TimeStopActive() override;

	/** TimeStop에 영향을 받지 않을 때 실행하는 함수입니다. */
	virtual void TimeStopDeactive() override;

	/** ParticleEffect 에셋을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRParticleEffect")
	UParticleSystem* GetParticleEffectAsset() const;

	/** 입력받은 인자로 ParticleEffect의 CustomTimeDilation을 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRParticleEffect")
	void SetParticleEffectCustomTimeDilation(float NewCustomTimeDilation);

private:
	/** Spawn한 ParticleSystemComponent입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffect", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleEffect;

public:
	/** ParticleEffect를 반환하는 함수입니다. */
	UParticleSystemComponent* GetParticleEffect() const;
	
	/** 입력받은 인자로 ParticleEffect를 설정하는 함수입니다. */
	void SetParticleEffect(UParticleSystemComponent* NewParticleEffect);

public:
	/** ParticleEffect를 비활성화하는 델리게이트입니다. */
	FOnParticleEffectDeactivate OnParticleEffectDeactivate;
};
