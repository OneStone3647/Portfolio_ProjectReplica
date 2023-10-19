// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRObjectPoolSystemComponent.h"
#include "Objects/PRPooledObject.h"
#include "Characters/PRBaseCharacter.h"

UPRObjectPoolSystemComponent::UPRObjectPoolSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PooledObjectInfos.Empty();
	ObjectPool.Empty();
	ActivatePoolIndexes.Empty();
	bIgnoreTimeStop = false;
	bActivateTimeStop = false;
}

void UPRObjectPoolSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeObjectPool();
}

void UPRObjectPoolSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TimeStop을 무시할 때 Effect의 TimeDilation을 최신화합니다.
	if(bIgnoreTimeStop)
	{
		if(bActivateTimeStop)
		{
			UpdateObjectPoolsTimeDilation(DeltaTime);
		}
		else
		{
			UpdateObjectPoolsTimeDilation(GetPROwner()->GetWorld()->DeltaTimeSeconds * 0.1f);
		}
	}
}

void UPRObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// 모든 ObjectPool 파괴
	if(ObjectPool.Num() > 0)
	{
		for(auto& PooledObjects : ObjectPool)
		{
			for(auto& PooledObject : PooledObjects.Value.Objects)
			{
				PooledObject->Destroy();
			}
		}

		ObjectPool.Empty();
	}
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPRObjectPoolSystemComponent::CreateObjectPool(FPRPooledObjectInfo PooledObjectInfo)
{
	if(GetOwner() != nullptr && PooledObjectInfo.PooledObjectClass != nullptr)
	{
		FPRObjectPool NewObjectPool;
		for(int32 Index = 0; Index < PooledObjectInfo.PoolSize; Index++)
		{
			APRPooledObject* PoolableObject = GetOwner()->GetWorld()->SpawnActor<APRPooledObject>(PooledObjectInfo.PooledObjectClass);
			if(IsValid(PoolableObject) == true)
			{
				PoolableObject->SetObjectOwner(GetOwner());
				PoolableObject->InitializePooledObject();
				PoolableObject->Deactivate();
				PoolableObject->SetObjectName(PooledObjectInfo.ObjectName);
				PoolableObject->SetLifespan(PooledObjectInfo.Lifespan);
				PoolableObject->SetPoolIndex(Index);
				PoolableObject->OnPooledObjectDeactivate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnPooledObjectDeactivate);
				NewObjectPool.Objects.Emplace(PoolableObject);
			}
		}

		ObjectPool.Emplace(PooledObjectInfo.PooledObjectClass, NewObjectPool);
	}
}

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObject(TSubclassOf<APRPooledObject> NewObjectPool)
{
	// 오브젝트 풀에서 실행할 오브젝트를 가진 오브젝트 그룹을 탐색합니다.
	for(auto& PooledObjects : ObjectPool)
	{
		if(PooledObjects.Key == NewObjectPool)
		{
			// 활성화되지 않은 오브젝트를 찾아 활성화합니다.
			for(auto& ActivateableObject : PooledObjects.Value.Objects)
			{
				if(IsValid(ActivateableObject) == true && IsActivatePooledObject(ActivateableObject) == false)
				{
					ActivateableObject->Activate();
					// 처음 활성화하는 오브젝트의 경우 맵을 새로 추가합니다.
					if(ActivatePoolIndexes.Contains(NewObjectPool) == false)
					{
						ActivatePoolIndexes.Add(NewObjectPool);
					}
					ActivatePoolIndexes.Find(NewObjectPool)->ActivateIndexes.Add(ActivateableObject->GetPoolIndex());

					return ActivateableObject;
				}
			}

			// 오브젝트 그룹을 찾은 경우 다음 탐색을 진행하지 않습니다.
			break;
		}
	}

	// 찾고자하는 오브젝트가 모두 활성화되었을 경우 가장 먼저 활성화된 오브젝트를 비활성화하고 다시 활성화한 다음 반환합니다.
	if(ActivatePoolIndexes.Num() > 0)
	{
		for(auto& ActivatePoolIndex : ActivatePoolIndexes)
		{
			// 가장 먼저 활성화된 오브젝트의 Index를 제거합니다.
			if(ActivatePoolIndex.Key == NewObjectPool
				&& ActivatePoolIndex.Value.ActivateIndexes.IsValidIndex(0) == true)
			{
				const int32 PooledObjectIndex = ActivatePoolIndex.Value.ActivateIndexes[0];
				ActivatePoolIndex.Value.ActivateIndexes.Remove(PooledObjectIndex);

				// 가장 먼저 활성화된 오브젝트를 비활성화하고 다시 활성화하고 반환합니다.				
				for(auto& PooledObjects : ObjectPool)
				{
					if(PooledObjects.Key == NewObjectPool
						&& PooledObjects.Value.Objects.IsValidIndex(PooledObjectIndex) == true)
					{
						APRPooledObject* NewPooledObject = PooledObjects.Value.Objects[PooledObjectIndex];
						if(IsValid(NewPooledObject) == true)
						{
							NewPooledObject->Deactivate();
							NewPooledObject->Activate();
							ActivatePoolIndex.Value.ActivateIndexes.Emplace(NewPooledObject->GetPoolIndex());

							return NewPooledObject;
						}

						return nullptr;
					}
				}

				return nullptr;
			}
		}
	}
	
	return nullptr;
}

void UPRObjectPoolSystemComponent::OnPooledObjectDeactivate(APRPooledObject* PooledObject)
{
	for(auto& ActivatePoolIndex : ActivatePoolIndexes)
	{
		if(ActivatePoolIndex.Key == PooledObject->GetClass())
		{
			ActivatePoolIndex.Value.ActivateIndexes.Remove(PooledObject->GetPoolIndex());
			return;
		}
	}
}

bool UPRObjectPoolSystemComponent::IsActivatePooledObject(APRPooledObject* PooledObject)
{
	for(auto& ActivatePoolIndex : ActivatePoolIndexes)
	{
		if(ActivatePoolIndex.Key == PooledObject->GetClass())
		{
			for(const auto& Index : ActivatePoolIndex.Value.ActivateIndexes)
			{
				if(Index == PooledObject->GetPoolIndex())
				{
					return true;
				}
			}

			return false;
		}
	}

	return false;
}

bool UPRObjectPoolSystemComponent::IsCreatePooledObject(TSubclassOf<APRPooledObject> NewObjectPool)
{
	return ObjectPool.Contains(NewObjectPool);
}

void UPRObjectPoolSystemComponent::AddPooledObjectInfo(FPRPooledObjectInfo NewPooledObjectInfo)
{
	PooledObjectInfos.Emplace(NewPooledObjectInfo);
}

void UPRObjectPoolSystemComponent::InitializeObjectPool()
{
	// ObjectPool 초기화
	if(ObjectPool.Num() > 0)
	{
		for(auto& PooledObjects : ObjectPool)
		{
			for(auto& PooledObject : PooledObjects.Value.Objects)
			{
				PooledObject->Destroy();
			}
		}

		ObjectPool.Empty();
	}

	// ActivatePoolIndexes 초기화
	if(ActivatePoolIndexes.Num() > 0)
	{
		ActivatePoolIndexes.Empty();
	}

	// ObjectInfo를 바탕으로 ObjectPool 생성
	if(GetOwner()->GetWorld() != nullptr && PooledObjectInfos.Num() > 0)
	{
		for(const auto& PooledObjectInfo : PooledObjectInfos)
		{
			CreateObjectPool(PooledObjectInfo);
		}
	}
}

void UPRObjectPoolSystemComponent::UpdateObjectPoolsTimeDilation(float DeltaTime)
{
	for(auto& PooledObjects : ObjectPool)
	{
		for(auto& PooledObject : PooledObjects.Value.Objects)
		{
			PooledObject->UpdatePooledObject(DeltaTime);
		}
	}
}

void UPRObjectPoolSystemComponent::SetIgnoreTimeStop(bool bNewIgnoreTimeStop)
{
	bIgnoreTimeStop = bNewIgnoreTimeStop;
}

void UPRObjectPoolSystemComponent::SetActivateTimeStop(bool bNewActivateTimeStop)
{
	bActivateTimeStop = bNewActivateTimeStop;
}
