// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PREffectSystemComponent.generated.h"

class UPREffect;
class UPRNiagaraEffect;
class UPRParticleEffect;
class UFXSystemAsset;
class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystemComponent;

/** 생성할 Niagara Effect의 정보를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** 생성할 NiagaraSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectInfo")
	UNiagaraSystem* NiagaraSystem;

	/** 풀에 넣을 초기에 생성할 NiagaraEffect의 수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectInfo")
	int32 PoolSize;

	/** Effect의 수명입니다. Effect의 수명이 끝날 경우 Effect를 비활성화합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectInfo")
	float Lifespan;	
};

/** 생성할 Particle Effect의 정보를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** 생성할 ParticleSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectInfo")
	UParticleSystem* ParticleSystem;

	/** 풀에 넣을 초기에 생성할 ParticleEffect의 수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectInfo")
	int32 PoolSize;

	/** Effect의 수명입니다. Effect의 수명이 끝날 경우 Effect를 비활성화합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectInfo")
	float Lifespan;	
};

/** NiagaraEffect를 넣은 풀의 정보를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectPool
{
	GENERATED_BODY()

public:
	/** NiagaraEffect를 넣은 풀입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectPool")
	TArray<class UPRNiagaraEffect*> Effects;
};

/** ParticleEffect를 넣은 풀의 정보를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectPool
{
	GENERATED_BODY()

public:
	/** ParticleEffect를 넣은 풀입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectPool")
	TArray<class UPRParticleEffect*> Effects;
};

/** EffectPool의 활성화된 Effect의 Index 정보를 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateIndex
{
	GENERATED_BODY()

public:
	/** 활성화된 Effect의 Index입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectPool")
	TArray<int32> ActivateIndexes;
};

/**
 * Owner가 사용하는 Effect를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPREffectSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPREffectSystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** EffectSystem을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void Initialize();

	/** EffectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void CreateEffectPool();

	// /** Effect를 지정한 위치에 Spawn하는 함수입니다. */
	// UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	// UPREffect* SpawnEffectAtLocation(UFXSystemAsset* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true);
	//
	// /** Effect를 지정한 Component에 Spawn하여 부착하는 함수입니다. */
	// UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	// UPREffect* SpawnEffectAttached(UFXSystemAsset* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate = true);

	/** NiagaraEffect를 지정한 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRNiagaraEffect* SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true);

	/** NiagaraEffect를 지정한 Component에 Spawn하여 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRNiagaraEffect* SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate = true);

	/** ParticleEffect를 지정한 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRParticleEffect* SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true);

	/** ParticleEffect를 지정한 Component에 Spawn하여 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRParticleEffect* SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate = true);

	/** 활성화된 Effect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsActivateEffect(UPREffect* Effect) const;

	/** EffectPool에 존재하는 Effect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsValidEffectPool(UFXSystemAsset* FXSystemAsset) const;

	/** NiagaraEffectPool에 존재하는 Effect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsValidNiagaraEffectPool(UNiagaraSystem* NewNiagaraSystem) const;
	
	/** ParticleEffectPool에 존재하는 Effect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsValidParticleEffectPool(UParticleSystem* NewParticleSystem) const;

	/** 입력받은 인자를 바탕으로 NiagaraEffect를 생성하고 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRNiagaraEffect* CreateNiagaraEffect(UNiagaraSystem* NiagaraSystem);

	/** 입력받은 인자를 바탕으로 ParticleEffect를 생성하고 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRParticleEffect* CreateParticleEffect(UParticleSystem* ParticleSystem);
	
	/** 입력받은 인자를 바탕으로 NiagaraEffect를 Spawn하여 담은 풀을 반환하는 함수입니다.*/
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	FPRNiagaraEffectPool CreateNiagaraEffectPool(FPRNiagaraEffectInfo NiagaraEffectInfo);

	/** 입력받은 인자를 바탕으로 ParticleEffect를 Spawn하여 담은 풀을 반환하는 함수입니다.*/
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	FPRParticleEffectPool CreateParticleEffectPool(FPRParticleEffectInfo ParticleEffectInfo);

	// /** 입력받은 인자를 바탕으로 NiagaraEffect를 Spawn하여 담은 풀을 반환하는 함수입니다.*/
	// UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	// FPREffectPool CreateNiagaraEffectPool(FPRNiagaraEffectInfo NiagaraEffectInfo);
	//
	// /** 입력받은 인자를 바탕으로 ParticleEffect를 Spawn하여 담은 풀을 반환하는 함수입니다.*/
	// UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	// FPREffectPool CreateParticleEffectPool(FPRParticleEffectInfo ParticleEffectInfo);

	/** 인자로 받은 Effect를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void OnEffectDeactivate(UPREffect* DeactivateEffect);

	/** Effect의 TimeDilation을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void UpdateEffectTimeDilation(float DeltaTime = 0.0f);

	/** Effect의 CustomDepth를 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void SetEffectsCustomDepth(bool bNewRenderCustomDepth, int32 NewCustomDepthStencilValue);

	/** 인자로 받은 Effect의 EffectPool을 모두 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DeactivateEffectPool(UFXSystemAsset* NewDeactivateEffect);

	/** 인자로 받은 NiagaraEffect의 EffectPool을 모두 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DeactivateNiagaraEffectPool(UNiagaraSystem* NewDeactivateNiagaraEffect);

	/** 인자로 받은 ParticleEffect의 EffectPool을 모두 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DeactivateParticleEffectPool(UParticleSystem* NewDeactivateParticleEffect);

	/** TimeStop에 영향을 받았는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	bool IsTimeStopActive() const;

	/** TimeStop에 영향을 받을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void TimeStopActive();

	/** TimeStop에 영향을 받지 않을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void TimeStopDeactive();

private:
	// /** Effect를 넣은 풀입니다. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	// TMap<UFXSystemAsset*, FPREffectPool> EffectPool;
	//
	// /** 활성화된 Effect의 Index입니다. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	// TMap<UFXSystemAsset*, FPRActivateIndex> ActivatePoolIndexes;

	/** NiagaraEffect를 넣은 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRNiagaraEffectPool> NiagaraEffectPool;
	
	/** 활성화된 NiagaraEffect의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRActivateIndex> ActivateNiagaraEffectIndexes;
	
	/** ParticleEffect를 넣은 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRParticleEffectPool> ParticleEffectPool;
	
	/** 활성화된 ParticleEffect의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRActivateIndex> ActivateParticleEffectIndexes;

	// /** TimeStop을 무시하고 Effect를 재생하는지 나타내는 변수입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	// bool bIgnoreTimeStop;

	/** TimeStop에 영향을 받는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	bool bTimeStopActive;
	
public:
	// /** 입력받은 인자로 bIgnoreTimeStop을 설정하는 함수입니다. */
	// void SetIgnoreTimeStop(bool bNewIgnoreTimeStop);

	// /** 입력받은 인자로 bActivateTimeStop을 설정하는 함수입니다. */
	// UFUNCTION()
	// void SetActivateTimeStop(bool bNewActivateTimeStop);

#pragma region DataTable
private:
	/** NiagaraEffect를 보관한 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* NiagaraEffectDataTable;

	/** ParticleEffect를 보관한 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ParticleEffectDataTable;
#pragma endregion 
};
