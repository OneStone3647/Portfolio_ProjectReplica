// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRObjectPoolSystemComponent.generated.h"

class APRPooledObject;

/**
 * 오브젝트 풀에 넣을 오브젝트의 정보를 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRPooledObjectInfo
{
	GENERATED_BODY()

public:
	FPRPooledObjectInfo()
		: ObjectName(FName())
		, PooledObjectClass(nullptr)
		, PoolSize(0)
		, Lifespan(0.0f)
	{}

	FPRPooledObjectInfo(FName NewObjectName, TSubclassOf<APRPooledObject> NewPooledObjectClass, int32 NewPoolSize, float NewLifespan)
		: ObjectName(NewObjectName)
		, PooledObjectClass(NewPooledObjectClass)
		, PoolSize(NewPoolSize)
		, Lifespan(NewLifespan)
	{}

public:
	/** 오브젝트의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPooledObjectInfo")
	FName ObjectName;

	/** 풀에 넣을 오브젝트의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPooledObjectInfo")
	TSubclassOf<APRPooledObject> PooledObjectClass;

	/** 풀에 넣는 오브젝트의 수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPooledObjectInfo")
	int32 PoolSize;

	/** 오브젝트의 수명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPooledObjectInfo")
	float Lifespan;
};

/**
 * 오브젝트를 넣은 풀의 정보를 가진 구조체입니다.
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
	/** 오브젝트를 넣은 풀입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPooledObjectPool")
	TArray<class APRPooledObject*> Objects;
};

/**
 * 활성화된 오브젝트의 Index정보를 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRPoolIndex
{
	GENERATED_BODY()

public:
	FPRPoolIndex()
		: ActivateIndexes()
	{}

	FPRPoolIndex(TArray<int32> NewActivateIndexes)
		: ActivateIndexes(NewActivateIndexes)
	{}
	

public:
	/** 활성화된 오브젝트의 Index입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPoolIndex")
	TArray<int32> ActivateIndexes;
};

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
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** 인자로 받은 정보를 바탕으로 오브젝트 풀을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void CreateObjectPool(FPRPooledObjectInfo PooledObjectInfo);
	
	/** 풀에서 오브젝트 이름에 해당하는 오브젝트를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	APRPooledObject* ActivatePooledObject(FName NewObjectName);

	/** 인자로 받은 풀 오브젝트를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	void OnPooledObjectDeactivate(class APRPooledObject* PooledObject);

	/** 오브젝트 풀에서 활성화된 오브젝트인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsActivatePooledObject(class APRPooledObject* PooledObject);

	/** 인자로 받은 이름으로 생성된 오브젝트 풀이 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category  = "ObjectPoolSystem")
	bool IsCreatePooledObject(FName ObjectPoolName);

	/** 입력받은 인자를 PooledObjectInfos Array에 추가하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void AddPooledObjectInfo(FPRPooledObjectInfo NewPooledObjectInfo);

protected:
	/** 오브젝트 풀을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void InitializeObjectPool();

	/** 오브젝트 풀의 TimeDilation을 최신화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPoolSystem")
	void UpdateObjectPoolsTimeDilation(float DeltaTime = 0.0f);

protected:
	/** 오브젝트 풀에 넣기 위한 오브젝트의 클래스의 정보를 가진 구조체를 보관한 Array입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ObjectPoolSystem")
	TArray<FPRPooledObjectInfo> PooledObjectInfos;

	/** 오브젝트 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem")
	TMap<FName, FPRObjectPool> ObjectPool;

	/** 활성화한 오브젝트의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ObjectPoolSystem")
	TMap<FName, FPRPoolIndex> ActivatePoolIndexes;

	/** TimeStop을 무시하고 Object를 실행하는지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	bool bIgnoreTimeStop;

	/** TimeStop의 실행을 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	bool bActivateTimeStop;

public:
	/** 입력받은 인자로 bIgnoreTimeStop을 설정하는 함수입니다. */
	void SetIgnoreTimeStop(bool bNewIgnoreTimeStop);
	
	/** 입력받은 인자로 bActivateTimeStop을 설정하는 함수입니다. */
	UFUNCTION()
	void SetActivateTimeStop(bool bNewActivateTimeStop);
};
