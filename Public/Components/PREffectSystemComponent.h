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

/**
 * Niagara Effect Pool의 설정을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRNiagaraEffectSettings()
		: NiagaraSystem(nullptr)
		, PoolSize(0)
	{}

	FPRNiagaraEffectSettings(UNiagaraSystem* NewNiagaraSystem, int32 NewPoolSize)
		: NiagaraSystem(NewNiagaraSystem)
		, PoolSize(NewPoolSize)
	{}
	
public:
	/** 생성할 NiagaraSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectSettings")
	UNiagaraSystem* NiagaraSystem;

	/** 풀에 넣을 초기에 생성할 NiagaraEffect의 수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRNiagaraEffectSettings")
	int32 PoolSize;
};

/**
 * EffectPool에 넣은 NiagaraEffect의 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRNiagaraEffectPool
{
	GENERATED_BODY()

public:
	/** NiagaraEffect를 넣은 풀입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectList")
	TArray<class UPRNiagaraEffect*> Effects;
};

/**
 * 동적으로 생성한 NiagaraEffect와 NiagaraEffect를 제거할 때 사용하는 TimerHandle을 보관한 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyNiagaraEffectList
{
	GENERATED_BODY()
	
public:
	/** 동적으로 생성한 NiagaraEffect와 NiagaraEffect를 제거할 때 사용하는 TimerHandle입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyNiagaraEffect")
	TMap<UPRNiagaraEffect*, FTimerHandle> Effects;
};

/**
 * Particle Effect Pool의 설정을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRParticleEffectSettings()
		: ParticleSystem(nullptr)
		, PoolSize(0)
	{}

	FPRParticleEffectSettings(UParticleSystem* NewParticleSystem, int32 NewPoolSize)
		: ParticleSystem(NewParticleSystem)
		, PoolSize(NewPoolSize)
	{}
	
public:
	/** 생성할 ParticleSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectSettings")
	UParticleSystem* ParticleSystem;

	/** 풀에 넣을 초기에 생성할 ParticleEffect의 수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRParticleEffectSettings")
	int32 PoolSize;
};

/**
 * EffectPool에 넣은 ParticleEffect의 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRParticleEffectPool
{
	GENERATED_BODY()

public:
	/** ParticleEffect를 넣은 풀입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PREffectList")
	TArray<class UPRParticleEffect*> Effects;
};

/**
 * 동적으로 생성한 ParticleEffect와 ParticleEffect를 제거할 때 사용하는 TimerHandle을 보관한 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyParticleEffectList
{
	GENERATED_BODY()
	
public:
	/** 동적으로 생성한 ParticleEffect와 ParticleEffect를 제거할 때 사용하는 TimerHandle입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyParticleEffect")
	TMap<UPRParticleEffect*, FTimerHandle> Effects;
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren) override;
	
public:
	/** EffectPool을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void InitializeEffectPool();

	/** 모든 EffectPool을 제거하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DestroyAllEffectPool();
	
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
	/**
	 * 사용 가능한 Index를 찾아 반환하는 함수입니다.
	 *
	 * @param UsedIndexes 이미 사용 중인 Index 목록
	 * @return 사용 가능한 Index
	 */
	UFUNCTION(BlueprintCallable, Category  = "EffectSystem")
	int32 FindAvailableIndex(const TSet<int32>& UsedIndexes);

private:
	/** TimeStop에 영향을 받는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	bool bTimeStopActive;
	
	/** 동적으로 생성한 오브젝트를 제거하는 딜레이 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	float DynamicDestroyDelay;

#pragma region NiagaraEffect
public:
	/** NiagaraEffect를 지정한 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRNiagaraEffect* SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true);

	/** NiagaraEffect를 지정한 Component에 Spawn하여 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRNiagaraEffect* SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate = true);

	/** 인자로 받은 NiagaraEffect의 EffectPool을 모두 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DeactivateNiagaraEffectPool(UNiagaraSystem* NiagaraEffect);
	
	/** 활성화된 NiagaraEffect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	bool IsActivateNiagaraEffect(UPRNiagaraEffect* NiagaraEffect) const;
	
	/** 인자에 해당하는 NiagaraSystem의 Pool이 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	bool IsCreateNiagaraEffectPool(UNiagaraSystem* NiagaraSystem) const;

	/** 인자에 해당하는 NiagaraSystem의 ActivateIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	bool IsCreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem) const;

	/** 인자에 해당하는 NiagaraSystem의 UsedIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	bool IsCreateUsedNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem) const;
	
private:
	/** 인자에 해당하는 활성화할 수 있는 NiagaraSystem을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	UPRNiagaraEffect* GetActivateableNiagaraEffect(UNiagaraSystem* NiagaraSystem);
	
	/** NiagaraEffect의 TimeDilation을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void UpdateNiagaraEffectTimeDilation(float TimeDilation = 0.0f);

	/**
	 * 인자로 받은 NiagaraSystem을 월드에 Spawn하는 함수입니다.
	 *
	 * @param NiagaraSystem 월드에 Spawn할 NiagaraSystem
	 * @return 월드에 Spawn한 NiagaraEffect
	 */	
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	UPRNiagaraEffect* SpawnNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem);

	/**
	 * NiagaraEffect를 월드에 Spawn하고 초기화하는 함수입니다.
	 *
	 * @param NiagaraSystem 월드에 Spawn할 NiagaraSystem
	 * @param Index 월드에 Spawn한 NiagaraEffectPool의 NiagaraList의 Index
	 * @return 월드에 Spawn한 NiagaraEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	UPRNiagaraEffect* SpawnAndInitializeNiagaraEffect(UNiagaraSystem* NiagaraSystem, int32 Index = 0);
	
	/** 인자로 받은 ObjectPool의 설정을 바탕으로 ObjectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void CreateNiagaraEffectPool(FPRNiagaraEffectSettings NiagaraEffectSettings);
	
	/** 인자로 받은 NiagaraSystem의 ActivateNiagaraEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void CreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem);
	
	/** 인자로 받은 NiagaraEffect를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void OnNiagaraEffectDeactivate(UPRNiagaraEffect* NiagaraEffect);

	/**
	 * 동적으로 생성한 NiagaraEffect를 비활성화하는 함수입니다.
	 * NiagaraEffect의 OnDynamicNiagaraEffectActivate 델리게이트에 바인딩합니다.
	 *
	 * @param NiagaraEffect 비활성화할 NiagaraEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void OnDynamicNiagaraEffectDeactivate(UPRNiagaraEffect* NiagaraEffect);

	/**
	 * 동적으로 생성한 NiagaraEffect를 제거하는 함수입니다.
	 *
	 * @param NiagaraEffect 제거할 NiagaraEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	void OnDynamicNiagaraEffectDestroy(UPRNiagaraEffect* NiagaraEffect);

private:
	/** NiagaraEffectPool의 설정 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	UDataTable* NiagaraEffectSettingsDataTable;

	/** 월드에 Spawn한 NiagaraEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRNiagaraEffectPool> NiagaraEffectPool;

	/** 활성화된 NiagaraEffect의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRActivateIndexList> ActivateNiagaraEffectIndexList;	
	
	/**
	 * NiagaraEffect를 PoolSize보다 많이 동적으로 생성할 때 사용한 NiagaraEffectPool의 Index의 목록입니다.
	 * 동적으로 생성하는 NiagaraEffectPool의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRUsedIndexList> UsedNiagaraEffectIndexList;

	/** 동적으로 제거할 NiagaraEffect의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UNiagaraSystem*, FPRDynamicDestroyNiagaraEffectList> DynamicDestroyNiagaraEffectList;
#pragma endregion

#pragma region ParticleEffect
public:
	/** ParticleEffect를 지정한 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRParticleEffect* SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.0f), bool bEffectAutoActivate = true);

	/** ParticleEffect를 지정한 Component에 Spawn하여 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	UPRParticleEffect* SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate = true);

	/** 인자로 받은 ParticleEffect의 EffectPool을 모두 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem")
	void DeactivateParticleEffectPool(UParticleSystem* ParticleEffect);
	
	/** 활성화된 ParticleEffect인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	bool IsActivateParticleEffect(UPRParticleEffect* ParticleEffect) const;
	
	/** 인자에 해당하는 ParticleSystem의 Pool이 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	bool IsCreateParticleEffectPool(UParticleSystem* ParticleSystem) const;

	/** 인자에 해당하는 ParticleSystem의 ActivateIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	bool IsCreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem) const;

	/** 인자에 해당하는 ParticleSystem의 UsedIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	bool IsCreateUsedParticleEffectIndexList(UParticleSystem* ParticleSystem) const;
	
private:
	/** 인자에 해당하는 활성화할 수 있는 ParticleSystem을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	UPRParticleEffect* GetActivateableParticleEffect(UParticleSystem* ParticleSystem);
	
	/**
	 * 인자로 받은 ParticleSystem을 월드에 Spawn하는 함수입니다.
	 *
	 * @param ParticleSystem 월드에 Spawn할 ParticleSystem
	 * @return 월드에 Spawn한 ParticleEffect
	 */	
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	UPRParticleEffect* SpawnParticleEffectInWorld(UParticleSystem* ParticleSystem);

	/**
	 * ParticleEffect를 월드에 Spawn하고 초기화하는 함수입니다.
	 *
	 * @param ParticleSystem 월드에 Spawn할 ParticleSystem
	 * @param Index 월드에 Spawn한 ParticleEffectPool의 ParticleList의 Index
	 * @return 월드에 Spawn한 ParticleEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	UPRParticleEffect* SpawnAndInitializeParticleEffect(UParticleSystem* ParticleSystem, int32 Index = 0);
	
	/** 인자로 받은 ParticleEffectPool의 설정을 바탕으로 ParticleEffectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	void CreateParticleEffectPool(FPRParticleEffectSettings ParticleEffectSettings);
	
	/** 인자로 받은 ParticleSystem의 ActivateParticleEffectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	void CreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem);
	
	/** 인자로 받은 ParticleEffect를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	void OnParticleEffectDeactivate(UPRParticleEffect* ParticleEffect);

	/**
	 * 동적으로 생성한 ParticleEffect를 비활성화하는 함수입니다.
	 * ParticleEffect의 OnDynamicParticleEffectActivate 델리게이트에 바인딩합니다.
	 *
	 * @param ParticleEffect 비활성화할 ParticleEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	void OnDynamicParticleEffectDeactivate(UPRParticleEffect* ParticleEffect);

	/**
	 * 동적으로 생성한 ParticleEffect를 제거하는 함수입니다.
	 *
	 * @param ParticleEffect 제거할 ParticleEffect
	 */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	void OnDynamicParticleEffectDestroy(UPRParticleEffect* ParticleEffect);

private:
	/** ParticleEffectPool의 설정 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	UDataTable* ParticleEffectSettingsDataTable;

	/** 월드에 Spawn한 ParticleEffect의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRParticleEffectPool> ParticleEffectPool;

	/** 활성화된 ParticleEffect의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRActivateIndexList> ActivateParticleEffectIndexList;

	/**
	 * ParticleEffect를 PoolSize보다 많이 동적으로 생성할 때 사용한 ParticleEffectPool의 Index의 목록입니다.
	 * 동적으로 생성하는 ParticleEffectPool의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRUsedIndexList> UsedParticleEffectIndexList;

	/** 동적으로 제거할 ParticleEffect의 목록입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem|ParticleEffect", meta = (AllowPrivateAccess = "true"))
	TMap<UParticleSystem*, FPRDynamicDestroyParticleEffectList> DynamicDestroyParticleEffectList;
#pragma endregion
};
