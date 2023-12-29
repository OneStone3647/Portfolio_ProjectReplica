// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRObjectPoolSystemComponent.h"
#include "Objects/PRPooledObject.h"
#include "Characters/PRAICharacter.h"

UPRObjectPoolSystemComponent::UPRObjectPoolSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 데이터 테이블을 초기화합니다.
	ObjectPoolSettingsDataTable = nullptr;

	// ObjectPool과 ActivateObjectIndexList, UsedObjectIndexList, DynamicDestroyObjectList를 초기화합니다.
	ObjectPool.Empty();
	ActivateObjectIndexList.Empty();
	UsedObjectIndexList.Empty();
	DynamicDestroyObjectList.Empty();

	// 동적으로 생성한 오브젝트를 제거하는 딜레이 시간을 초기화합니다.
	DynamicDestroyDelay = 60.0f;

	// 초기에는 TimeStop에 영향을 받지 않습니다.
	bTimeStopActive = false;
}

void UPRObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// 모든 ObjectPool을 제거합니다.
	if(ObjectPool.Num() > 0)
	{
		for(auto& Pair : ObjectPool)
		{
			for(auto& PooledObject : Pair.Value.Objects)
			{
				PooledObject->Destroy();
			}
		}

		ObjectPool.Empty();
	}

	Super::DestroyComponent(bPromoteChildren);
}

void UPRObjectPoolSystemComponent::InitializeObjectPool()
{
	ObjectPool.Empty();
	ActivateObjectIndexList.Empty();
	UsedObjectIndexList.Empty();
	DynamicDestroyObjectList.Empty();

	// ObjectPoolSettings 데이터 테이블을 기반으로 ObjectPool을 초기화합니다.
	if(ObjectPoolSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = ObjectPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRObjectPoolSettings* ObjectPoolSettings = ObjectPoolSettingsDataTable->FindRow<FPRObjectPoolSettings>(RowName, FString(""));
			if(ObjectPoolSettings != nullptr)
			{
				CreateObjectPool(*ObjectPoolSettings);
			}
		}
	}
}

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation, FRotator NewRotation)
{
	// 해당 오브젝트 클래스의 ObjectPool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateObjectPool(PooledObjectClass))
	{
		FPRObjectPoolSettings ObjectPoolSettings = FPRObjectPoolSettings(PooledObjectClass, 1);
		CreateObjectPool(ObjectPoolSettings);
	}

	// ObjectPool에서 해당 오브젝트 클래스의 Pool을 얻습니다.
	FPRObjectPool* Pair = ObjectPool.Find(PooledObjectClass);
	if(Pair == nullptr)
	{
		// 지정된 오브젝트 클래스가 없습니다.
		return nullptr;
	}

	APRPooledObject* ActivateablePooledObject = nullptr;

	// Pair에서 활성화 되지 않은 오브젝트를 찾아 활성화합니다.
	for(const auto& PooledObject : Pair->Objects)
	{
		if(IsValid(PooledObject) && !IsActivatePooledObject(PooledObject))
		{
			ActivateablePooledObject = PooledObject;
			break;
		}
	}

	// Pair의 모든 오브젝트가 활성화 되었을 경우 새로운 오브젝트를 생성합니다.
	if(ActivateablePooledObject == nullptr)
	{
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedObjectIndexList(PooledObjectClass))
		{
			FPRUsedIndexList NewUsedObjectIndexList;
			for(const auto& PooledObject : Pair->Objects)
			{
				NewUsedObjectIndexList.Indexes.Add(PooledObject->GetPoolIndex());
			}

			UsedObjectIndexList.Emplace(PooledObjectClass, NewUsedObjectIndexList);
		}

		// UsedObjectIndexList에서 해당 오브젝트 클래스가 사용중인 ObjectIndex 목록을 얻습니다.
		FPRUsedIndexList* NewUsedObjectIndexList = UsedObjectIndexList.Find(PooledObjectClass);
		if(NewUsedObjectIndexList == nullptr)
		{
			// 지정된 오브젝트 클래스가 업습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(NewUsedObjectIndexList->Indexes);

		// 사용 가능한 Index를 UsedObjectIndexList에 추가합니다.
		NewUsedObjectIndexList->Indexes.Add(NewIndex);
		
		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 오브젝트를 생성하고 초기화하며 사용 가능한 Index를 설정합니다.
		ActivateablePooledObject = SpawnAndInitializeObject(PooledObjectClass, NewIndex);
		ActivateablePooledObject->OnDynamicPooledObjectDeactivate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate);

		// 새로 생성한 오브젝트를 Pair에 추가합니다.
		Pair->Objects.Emplace(ActivateablePooledObject);
	}

	// DynamicObjectDestroyTimer를 정지합니다.
	FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(ActivateablePooledObject->GetClass());
	if(DynamicDestroyPair != nullptr)
	{
		FTimerHandle* DynamicObjectDestroyTimer = DynamicDestroyPair->Objects.Find(ActivateablePooledObject);
		if(DynamicObjectDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicObjectDestroyTimer);
		}
	}

	// 오브젝트를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
	if(NewLocation == FVector::ZeroVector && NewRotation == FRotator::ZeroRotator)
	{
		ActivateablePooledObject->InitializeSpawnLocation();
	}
	else
	{
		ActivateablePooledObject->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
	ActivateablePooledObject->Activate();

	// 해당 오브젝트 클래스로 처음 활성화하는 경우 ActivateObjectIndexList를 생성합니다.
	if(!IsCreateActivateObjectIndexList(PooledObjectClass))
	{
		CreateActivateObjectIndexList(PooledObjectClass);
	}

	// 활성화된 오브젝트의 Index를 ActivateObjectIndexList에 추가합니다.
	ActivateObjectIndexList.Find(PooledObjectClass)->Indexes.AddUnique(ActivateablePooledObject->GetPoolIndex());
	
	return ActivateablePooledObject;
}

void UPRObjectPoolSystemComponent::DeactivateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass)
{
	if(IsCreateObjectPool(PooledObjectClass) == true)
	{
		// PooledObjectClass에 해당하는 ObjectPool의 모든 Object들을 비활성화합니다.
		for(auto& PooledObject : ObjectPool.Find(PooledObjectClass)->Objects)
		{
			PooledObject->Deactivate();
		}
	}
}

bool UPRObjectPoolSystemComponent::IsActivatePooledObject(APRPooledObject* PooledObject) const
{
	const FPRActivateIndexList* NewActivateObjectIndexes = ActivateObjectIndexList.Find(PooledObject->GetClass());
	if(NewActivateObjectIndexes != nullptr)
	{
		return NewActivateObjectIndexes->Indexes.Contains(PooledObject->GetPoolIndex()) && PooledObject->IsActivate();
	}
	
	return false;
}

bool UPRObjectPoolSystemComponent::IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ObjectPool.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ActivateObjectIndexList.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return UsedObjectIndexList.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsTimeStopActive() const
{
	return bTimeStopActive;
}

void UPRObjectPoolSystemComponent::TimeStopActive()
{
	bTimeStopActive = true;

	// 오브젝트들의 CustomTimeDilation을 0.0f으로 설정합니다.
	// ObjectPool의 모든 항목을 순회합니다.
	for(const auto& Pair : ObjectPool)
	{
		for(const auto& Object : Pair.Value.Objects)
		{
			Object->CustomTimeDilation = 0.0f;
		}
	}
}

void UPRObjectPoolSystemComponent::TimeStopDeactive()
{
	bTimeStopActive = false;

	// 오브젝트들의 CustomTimeDilation을 1.0f으로 설정합니다.
	// ObjectPool의 모든 항목을 순회합니다.
	for(const auto& Pair : ObjectPool)
	{
		for(const auto& Object : Pair.Value.Objects)
		{
			Object->CustomTimeDilation = 1.0f;
		}
	}
}

int32 UPRObjectPoolSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	int32 NewIndex= 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass)
{
	if(GetWorld() != nullptr)
	{
		return GetWorld()->SpawnActor<APRPooledObject>(ObjectClass);
	}

	return nullptr;
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index)
{
	APRPooledObject* SpawnObject = SpawnObjectInWorld(ObjectClass);
	if(SpawnObject != nullptr)
	{
		// 생성한 오브젝트를 초기화하고 OnPooledObjectDeactivate를 바인딩합니다.
		SpawnObject->InitializePooledObject();
		SpawnObject->SetObjectOwner(GetOwner());
		SpawnObject->Deactivate();
		SpawnObject->SetPoolIndex(Index);
		SpawnObject->OnPooledObjectDeactivate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnPooledObjectDeactivate);
	}

	return SpawnObject;
}

void UPRObjectPoolSystemComponent::CreateObjectPool(FPRObjectPoolSettings ObjectPoolSettings)
{
	if(GetWorld() != nullptr && ObjectPoolSettings.PooledObjectClass != nullptr)
	{
		// ObjectPool에 추가할 Pair를 초기화하고 오브젝트를 생성하여 추가합니다.
		FPRObjectPool Pair;
		for(int32 Index = 0; Index < ObjectPoolSettings.PoolSize; Index++)
		{
			APRPooledObject* SpawnObject = SpawnAndInitializeObject(ObjectPoolSettings.PooledObjectClass, Index);
			if(SpawnObject != nullptr)
			{
				// 초기화된 오브젝트를 Pair에 추가합니다.
				Pair.Objects.Emplace(SpawnObject);
			}
		}

		// 초기화된 Pair를 ObjectPool에 추가하고 ActivateObjectIndexList를 생성합니다.
		ObjectPool.Emplace(ObjectPoolSettings.PooledObjectClass, Pair);
		CreateActivateObjectIndexList(ObjectPoolSettings.PooledObjectClass);
	}
}

void UPRObjectPoolSystemComponent::CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass)
{
	// ObjectClass의 ActivateObjectIndexList를 생성합니다
	ActivateObjectIndexList.Emplace(ObjectClass);
}

void UPRObjectPoolSystemComponent::OnPooledObjectDeactivate(APRPooledObject* PooledObject)
{
	if(IsActivatePooledObject(PooledObject))
	{
		// 비활성화된 오브젝트의 Index를 ActivateObjectIndexList에서 제거합니다.
		ActivateObjectIndexList.Find(PooledObject->GetClass())->Indexes.Remove(PooledObject->GetPoolIndex());
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate(APRPooledObject* PooledObject)
{
	if(DynamicDestroyDelay > 0)
	{
		// 타이머로 딜레이 만큼 시간이 지난 후 OnDynamicObjectDestroy 함수를 실행합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDestroy, PooledObject);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// 타이머를 DynamicDestroyObjectList에 추가합니다.
		FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(PooledObject->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Objects.Emplace(PooledObject, DynamicDestroyDelayTimerHandle);
		}
		else
		{
			FPRDynamicDestroyObjectList ObjectList;
			ObjectList.Objects.Emplace(PooledObject, DynamicDestroyDelayTimerHandle);
			DynamicDestroyObjectList.Emplace(PooledObject->GetClass(), ObjectList);
		}
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 OnDynamicObjectDestroy 함수를 실행합니다.
		OnDynamicObjectDestroy(PooledObject);
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDestroy(APRPooledObject* PooledObject)
{
	// 사용 중인 ObjectPoolIndexList를 가지고 있는지 확인합니다.
	if(IsCreateUsedObjectIndexList(PooledObject->GetClass()))
	{
		// DynamicObjectDestroyTimer를 제거합니다.
		FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(PooledObject->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Objects.Remove(PooledObject);
		}

		// 해당 오브젝트 클래스의 UsedObjectIndex를 얻습니다.
		FPRUsedIndexList* UsedIndexList = UsedObjectIndexList.Find(PooledObject->GetClass());
		if(UsedIndexList != nullptr)
		{
			// 사용중인 Index를 제거합니다.
			UsedIndexList->Indexes.Remove(PooledObject->GetPoolIndex());
		}

		// 해당 오브젝트 클래스의 ObjectPool이 생성되었는지 확인합니다.
		if(IsCreateObjectPool(PooledObject->GetClass()))
		{
			// ObjectPool에서 해당 오브젝트를 제거하고 파괴합니다.
			ObjectPool.Find(PooledObject->GetClass())->Objects.Remove(PooledObject);
			PooledObject->Destroy();
		}
	}
}
