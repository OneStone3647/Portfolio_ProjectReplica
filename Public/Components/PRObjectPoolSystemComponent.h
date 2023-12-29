// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRObjectPoolSystemComponent.generated.h"

class APRPooledObject;

/**
 * 오브젝트 풀의 설정을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPoolSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRObjectPoolSettings()
		: PooledObjectClass(nullptr)
		, PoolSize(0)
	{}

	FPRObjectPoolSettings(TSubclassOf<APRPooledObject> NewPooledObjectClass, int32 NewPoolSize)
		: PooledObjectClass(NewPooledObjectClass)
		, PoolSize(NewPoolSize)
	{}

public:
	/** 오브젝트의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	TSubclassOf<APRPooledObject> PooledObjectClass;

	/** ObjectPool의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPoolSettings")
	int32 PoolSize;
};

/**
 * 오브젝트 풀에 넣은 오브젝트의 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRObjectPool
{
	GENERATED_BODY()

public:
	FPRObjectPool()
		: Objects()
	{}

	FPRObjectPool(TArray<class APRPooledObject*> NewObjects)
		: Objects(NewObjects)
	{}
	

public:
	/** 오브젝트 풀에 넣은 오브젝트들입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRObjectPool")
	TArray<class APRPooledObject*> Objects;
};

/**
 * 동적으로 생성한 오브젝트와 오브젝트를 제거할 때 사용하는 TimerHandle을 보관한 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyObjectList
{
	GENERATED_BODY()
	
public:
	/** 동적으로 생성한 오브젝트와 오브젝트를 제거할 때 사용하는 TimerHandle입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyObject")
	TMap<APRPooledObject*, FTimerHandle> Objects;
};

// /**
//  * 활성화된 오브젝트의 Index를 보관한 목록을 나타내는 구조체입니다.
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRActivateObjectList
// {
// 	GENERATED_BODY()
//
// public:
// 	/** 활성화된 오브젝트의 Index를 보관한 목록입니다. */
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRActivateObjectList")
// 	TArray<int32> Indexes;
// };
//
// /**
//  * 이전에 사용된 오브젝트의 Index를 보관한 목록을 나타내는 구조체입니다.
//  */
// USTRUCT(Atomic, BlueprintType)
// struct FPRUsedObjectList
// {
// 	GENERATED_BODY()
//
// public:
// 	/** 이전에 사용된 Index를 추적하기 위한 Set입니다. */
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRUsedObjectList")
// 	TSet<int32> Indexes;
// };

/**
 * 오브젝트 풀을 관리하는 ActorComponent 클래스입니다. 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRObjectPoolSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRObjectPoolSystemComponent();

protected:
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** 오브젝트 풀을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void InitializeObjectPool();

	/**
	 * 인자에 해당하는 오브젝트 클래스를 ObjectPool에서 찾아 존재할 경우 활성화하고 인자로 받은 좌표와 회전값을 적용하는 함수입니다.
	 *
	 * @param PooledObjectClass ObjectPool에서 찾을 오브젝트의 클래스
	 * @param NewLocation 적용할 오브젝트의 좌표
	 * @param NewRotation 적용할 오브젝트의 회전값
	 * @return ObjectPool에서 찾은 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	APRPooledObject* ActivatePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation = FVector::ZeroVector, FRotator NewRotation = FRotator::ZeroRotator);

	/** 인자로 받은 오브젝트의 풀에 존재하는 오브젝트들을 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void DeactivateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass);
	
	/** 오브젝트 풀에서 활성화된 오브젝트인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsActivatePooledObject(APRPooledObject* PooledObject) const;

	/** 인자에 해당하는 오브젝트 클래스의 Pool이 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/** 인자에 해당하는 오브젝트 클래스의 ActivateObjectIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/** 인자에 해당하는 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const;

	/** TimeStop에 영향을 받았는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	bool IsTimeStopActive() const;
	
	/** TimeStop에 영향을 받을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void TimeStopActive();
	
	/** TimeStop에 영향을 받지 않을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void TimeStopDeactive();

private:
	/**
	 * 사용 가능한 Index를 찾아 반환하는 함수입니다.
	 *
	 * @param UsedIndexes 이미 사용 중인 Index 목록
	 * @return 사용 가능한 Index
	 */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	int32 FindAvailableIndex(const TSet<int32>& UsedIndexes);
	
	/**
	 * 인자로 받은 클래스에 해당하는 오브젝트를 월드에 Spawn하는 함수입니다.
	 *
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스
	 * @return 월드에 Spawn한 오브젝트
	 */	
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSytstem")
	APRPooledObject* SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass);

	/**
	 * 오브젝트를 월드에 Spawn하고 초기화하는 함수입니다.
	 *
	 * @param ObjectClass 월드에 Spawn할 오브젝트의 클래스
	 * @param Index 월드에 Spawn한 오브젝트의 ObjectPool의 ObjectList의 Index
	 * @return 월드에 Spawn한 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSytstem")
	APRPooledObject* SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index = 0);
	
	/** 인자로 받은 ObjectPool의 설정을 바탕으로 ObjectPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSytstem")
	void CreateObjectPool(FPRObjectPoolSettings ObjectPoolSettings);

	/** 인자로 받은 오브젝트 클래스의 ActivateObjectIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSytstem")
	void CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass);

	/** 인자로 받은 풀 오브젝트를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	void OnPooledObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 동적으로 생성한 오브젝트를 비활성화하는 함수입니다.
	 * 오브젝트의 OnDynamicObjectActivate 델리게이트에 바인딩합니다.
	 *
	 * @param PooledObject 비활성화할 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	void OnDynamicObjectDeactivate(APRPooledObject* PooledObject);

	/**
	 * 동적으로 생성한 오브젝트를 제거하는 함수입니다.
	 *
	 * @param PooledObject 제거할 오브젝트
	 */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	void OnDynamicObjectDestroy(APRPooledObject* PooledObject);

private:
	/** ObjectPool의 설정 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem|DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ObjectPoolSettingsDataTable;
	
	/** 월드에 Spawn한 오브젝트의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRObjectPool> ObjectPool;

	/** 활성화된 오브젝트의 Index 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRActivateIndexList> ActivateObjectIndexList;	
	
	/**
	 * 오브젝트를 PoolSize보다 많이 동적으로 생성할 때 사용한 오브젝트 풀의 Index의 목록입니다.
	 * 동적으로 생성하는 오브젝트 풀의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRUsedIndexList> UsedObjectIndexList;

	/** 동적으로 제거할 오브젝트의 목록입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRPooledObject>, FPRDynamicDestroyObjectList> DynamicDestroyObjectList;
	
	/** 동적으로 생성한 오브젝트를 제거하는 딜레이 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	float DynamicDestroyDelay;

	/** TimeStop에 영향을 받는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	bool bTimeStopActive;
};
