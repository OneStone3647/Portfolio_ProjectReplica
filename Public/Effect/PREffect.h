// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "UObject/NoExportTypes.h"
#include "PREffect.generated.h"

class UFXSystemAsset;
class UFXSystemComponent;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectDeactivate, UPREffect*, Effect);

/**
 * EffectSystem이 관리하는 Effect 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPREffect : public UObject
{
	GENERATED_BODY()

public:
	UPREffect();

public:
	/** Effect를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void Initialize();

	/** Effect를 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void UpdateEffect(float DeltaTime);

	/** Effect의 활성화를 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual bool IsActivate() const;

	/** Effect를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void Activate();

	/** Effect를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void Deactivate();

	/** Effect를 제거하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void Destroy();

	/** Effect를 지정한 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void SpawnEffectAtLocation(FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bAutoActivate = true);

	/** Effect를 지정한 Component에 부작하여 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual void SpawnEffectAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate = true);

	/** Effect의 위치를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual FVector GetEffectLocation() const;

	/** FXSystemComponent을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual UFXSystemComponent* GetFXSystemComponent() const;

	/** Effect가 반복되는 경우 true를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PREffect")
	virtual bool IsLooping() const;

	/** TimeStop에 영향을 받았는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsTimeStopActive() const;
	
	/** TimeStop에 영향을 받을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	virtual void TimeStopActive();

	/** TimeStop에 영향을 받지 않을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	virtual void TimeStopDeactive();

private:
	/** Effect의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	bool bActivate;

	// /** Effect 에셋입니다. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	// UFXSystemAsset* EffectSourceAsset;

	/** Effect의 소유자입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	AActor* EffectOwner;

	/** Effect의 PoolIndex입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	int32 PoolIndex;	

	/** Effect의 수명입니다. 수명이 다할 경우 Effect는 비활성화됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	float Lifespan;

	/** Effect의 수명을 관리하는 TimerHandle입니다. */
	FTimerHandle LifespanTimerHandle;

	/** TimeStop에 영향을 받는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PREffect", meta = (AllowPrivateAccess = "true"))
	bool bTimeStopActive;

public:
	// /** EffectSourceAsset을 반환하는 함수입니다. */
	// UFXSystemAsset* GetEffectSourceAsset() const;
	//
	// /** 입력받은 인자로 EffectSourceAsset을 설정하는 함수입니다. */
	// void SetEffectSourceAsset(UFXSystemAsset* NewEffectSourceAsset);

	/** EffectOwner를 반환하는 함수입니다. */
	AActor* GetEffectOwner() const;

	/** 입력받은 인자로 EffectOwner를 설정하는 함수입니다. */
	void SetEffectOwner(AActor* NewEffectOwner);
	
	/** PoolIndex를 반환하는 함수입니다. */
	int32 GetPoolIndex() const;
	
	/** 입력받은 인자로 PoolIndex를 설정하는 함수입니다. */
	void SetPoolIndex(int32 NewPoolIndex);

	/** Lifespan을 반환하는 함수입니다. */
	float GetLifeSpan() const;	
	
	/** 입력받은 인자로 Lifespan을 설정하는 함수입니다. */
	void SetLifespan(float NewLifespan);

// public:
// 	/** Effect를 비활성화하는 델리게이트입니다. */
// 	FOnEffectDeactivate OnEffectDeactivate;
};
