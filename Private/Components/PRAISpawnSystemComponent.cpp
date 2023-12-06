// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRAISpawnSystemComponent.h"
#include "Components/PRStatSystemComponent.h"
#include "ProjectReplicaGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

UPRAISpawnSystemComponent::UPRAISpawnSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AIPoolSettingsDataTable = nullptr;
	
	// AIPool와 ActivateAIList, UsedActivateAIListIndexes, DynamicAIDestroyTimers를 초기화합니다.
	AIPool.Empty();
	ActivateAIList.Empty();
	UsedAIListIndexes.Empty();
	DynamicAIDestroyTimers.Empty();

	// 동적으로 생성한 AI 캐릭터를 제거하는 딜레이 시간을 초기화합니다.
	DynamicDestroyDelay = 60.0f;
}

void UPRAISpawnSystemComponent::InitializeAISpawnSystem()
{
	AIPool.Empty();
	ActivateAIList.Empty();
	UsedAIListIndexes.Empty();
	DynamicAIDestroyTimers.Empty();
	
	// AIPoolSettings 데이터 테이블을 기반으로 AIPool을 초기화합니다.
	if(AIPoolSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = AIPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRAIPoolSettings* AIPoolSettings = AIPoolSettingsDataTable->FindRow<FPRAIPoolSettings>(RowName, FString(""));
			if(AIPoolSettings != nullptr)
			{
				CreateAIPool(*AIPoolSettings);
			}
		}
	}
}

APRAICharacter* UPRAISpawnSystemComponent::ActivateAI(TSubclassOf<APRAICharacter> AICharacterClass, FVector SpawnLocation,	FRotator SpawnRotation)
{
	// 해당 AI 캐릭터 클래스의 AIPool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateAIPool(AICharacterClass))
	{
		FPRAIPoolSettings AIPoolSettings = FPRAIPoolSettings(AICharacterClass, 1);
		CreateAIPool(AIPoolSettings);
	}

	// AIPool에서 해당 AI 캐릭터 클래스의 목록을 얻습니다.
	FPRAIList* AICharacterList = AIPool.Find(AICharacterClass);
	if(AICharacterList == nullptr)
	{
		// 지정된 AI 캐릭터 클래스가 없습니다.
		return nullptr;
	}

	APRAICharacter* ActivateableAICharacter = nullptr;
	// AICharacterList에서 활성화 되지 않은 AI 캐릭터를 찾아 활성화합니다.
	for(const auto& AICharacter : AICharacterList->List)
	{
		if(IsValid(AICharacter) && !IsActivateAI(AICharacter))
		{
			ActivateableAICharacter = AICharacter;
			break;
		}
	}

	// AICharacterList의 모든 AI 캐릭터가 활성화 되었을 경우 새로운 AI 캐릭터를 생성합니다.
	if(ActivateableAICharacter == nullptr)
	{
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 AI 캐릭터 클래스의 UsedAIListIndexes가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedAIListIndexes(AICharacterClass))
		{
			FPRUsedAIList NewUsedAIListIndexes;
			for(const auto& AICharacter : AICharacterList->List)
			{
				NewUsedAIListIndexes.Indexes.Add(AICharacter->GetAIListIndex());
			}
			
			UsedAIListIndexes.Emplace(AICharacterClass, NewUsedAIListIndexes);
		}

		// USedAIListIndexes에서 해당 AI 캐릭터 클래스가 사용중인 AIListIndex 목록을 얻습니다.
		FPRUsedAIList* UsedAICharacterListIndexes = UsedAIListIndexes.Find(AICharacterClass);
		if(UsedAICharacterListIndexes == nullptr)
		{
			// 지정된 AI 캐릭터 클래스가 없습니다.
			return nullptr;
		}

		// 사용 가능한 AIListIndex를 구합니다.
		const int32 NewAIListIndex = FindAvailableIndex(UsedAICharacterListIndexes->Indexes);

		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 AI 캐릭터를 생성하고 초기화하며 사용 가능한 AIListIndex를 설정합니다. OnDynamicAIDeactivate를 바인딩합니다.
		ActivateableAICharacter = SpawnAndInitializeAI(AICharacterClass, NewAIListIndex);
		ActivateableAICharacter->OnDynamicAIDeactivate.AddDynamic(this, &UPRAISpawnSystemComponent::OnDynamicAIDeactivate);

		// 사용 가능한 AIListIndex를 사용중인 AIListIndex 목록에 추가합니다.
		UsedAICharacterListIndexes->Indexes.Add(NewAIListIndex);

		// 새로 생성한 AI 캐릭터를 AICharacterList에 추가합니다.
		AICharacterList->List.Emplace(ActivateableAICharacter);
	}

	// DynamicAIDestroyTimer를 정지합니다.
	FTimerHandle* DynamicAIDestroyTimer = DynamicAIDestroyTimers.Find(ActivateableAICharacter);
	if(DynamicAIDestroyTimer != nullptr)
	{
		GetWorld()->GetTimerManager().PauseTimer(*DynamicAIDestroyTimer);
	}

	// AI 캐릭터를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
	ActivateableAICharacter->Activate();
	ActivateableAICharacter->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	
	// 해당 AI 캐릭터 클래스로 처음 활성화하는 경우 ActivateAICharacterList를 생성합니다.
	if(!IsCreateActivateAIList(AICharacterClass))
	{
		CreateActivateAIList(AICharacterClass);
	}
	
	// 활성화된 AI 캐릭터의 Index를 ActivateAIList에 추가합니다.
	ActivateAIList.Find(AICharacterClass)->Indexes.AddUnique(ActivateableAICharacter->GetAIListIndex());

	return ActivateableAICharacter;
}

bool UPRAISpawnSystemComponent::IsCreateAIPool(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return AIPool.Contains(AICharacterClass);
}

bool UPRAISpawnSystemComponent::IsCreateActivateAIList(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return ActivateAIList.Contains(AICharacterClass);
}

bool UPRAISpawnSystemComponent::IsCreateUsedAIListIndexes(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return UsedAIListIndexes.Contains(AICharacterClass);
}

bool UPRAISpawnSystemComponent::IsActivateAI(APRAICharacter* PooledAICharacter) const
{
	const FPRActivateAIList* ActivateAIIndexList = ActivateAIList.Find(PooledAICharacter->GetClass());
	if(ActivateAIIndexList != nullptr)
	{
		return ActivateAIIndexList->Indexes.Contains(PooledAICharacter->GetAIListIndex()) && PooledAICharacter->IsActivate();
	}

	return false;
}

void UPRAISpawnSystemComponent::OnAIDeactivate(APRAICharacter* PooledAICharacter)
{
	if(IsActivateAI(PooledAICharacter))
	{
		// 비활성화된 AI 캐릭터의 Index를 ActivateAIList에서 제거합니다.
		ActivateAIList.Find(PooledAICharacter->GetClass())->Indexes.Remove(PooledAICharacter->GetAIListIndex());
	}
}

TArray<APRAICharacter*> UPRAISpawnSystemComponent::GetAllActivateAICharacters() const
{
	TArray<APRAICharacter*> ActivateAICharacters;

	// AIPool의 모든 항목을 순회합니다.
	for(const auto& Pool : AIPool)
	{
		// 현재 AI 캐릭터의 클래스의 목록을 얻습니다.
		const TSubclassOf<APRAICharacter> AICharacterClass = Pool.Key;
		const FPRAIList AICharacterList = Pool.Value;

		// 현재 AI 캐릭터 클래스에 대한 활성화된 Index 목록을 얻습니다.
		const FPRActivateAIList* ActivateAIListIndexes = ActivateAIList.Find(AICharacterClass);
		if(ActivateAIListIndexes != nullptr && ActivateAIListIndexes->Indexes.Num() > 0)
		{
			// ActivateAIListIndexes에 있는 ListIndex에 해당하는 AICharacter를 ActivateAICharacters에 추가합니다.
			for(int32 ListIndex : ActivateAIListIndexes->Indexes)
			{
				for(const auto& AICharacter : AICharacterList.List)
				{
					if(AICharacter->GetAIListIndex() == ListIndex)
					{
						ActivateAICharacters.Emplace(AICharacter);
						break;
					}
				}
			}
		}
	}

	return ActivateAICharacters;
}

APRAICharacter* UPRAISpawnSystemComponent::SpawnAIInWorld(TSubclassOf<APRAICharacter> AICharacterClass)
{
	if(GetWorld() != nullptr)
	{
		return GetWorld()->SpawnActor<APRAICharacter>(AICharacterClass);
	}

	return nullptr;
}

APRAICharacter* UPRAISpawnSystemComponent::SpawnAndInitializeAI(TSubclassOf<APRAICharacter> AICharacterClass, int32 ListIndex)
{
	APRAICharacter* SpawnAICharacter = SpawnAIInWorld(AICharacterClass);
	if(SpawnAICharacter != nullptr)
	{
		// 기본 AI 컨트롤러를 Spawn합니다.
		// 동적으로 생성한 AI 캐릭터는 AI 컨트롤러를 직접 Spawn해야 캐릭터가 중력의 영향을 받습니다.
		SpawnAICharacter->SpawnDefaultController();
		
		// 생성된 AI 캐릭터를 비활성화하고, ListIndex를 설정하며, OnAIDeactivate를 바인딩합니다.
		SpawnAICharacter->Deactivate();
		SpawnAICharacter->SetAIListIndex(ListIndex);
		SpawnAICharacter->OnAIDeactivate.AddDynamic(this, &UPRAISpawnSystemComponent::OnAIDeactivate);

		// 게임 인스턴스를 통해 AI 캐릭터의 정보 데이터 테이블을 가져오고, 해당 클래스에 대한 정보를 찾아 초기화합니다.
		UProjectReplicaGameInstance* PRGameInstance = Cast<UProjectReplicaGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if(PRGameInstance != nullptr)
		{
			FPRCharacterStat DataTableCharacterStat = FPRCharacterStat();
			UDataTable* AIInfoDataTable = PRGameInstance->GetAIInfoDataTable();
			if(AIInfoDataTable != nullptr)
			{
				TArray<FName> RowNames = AIInfoDataTable->GetRowNames();
				for(const auto& RowName : RowNames)
				{
					FPRAICharacterInfo* DataTableRow = AIInfoDataTable->FindRow<FPRAICharacterInfo>(RowName, FString(""));

					// 데이터 테이블에서 클래스에 대한 정보를 찾았을 때 초기화합니다.
					if(DataTableRow != nullptr && AICharacterClass == DataTableRow->AICharacterClass)
					{
						DataTableCharacterStat = DataTableRow->CharacterStat;
					}
				}
			}

			// 캐릭터의 스탯을 초기화합니다.
			SpawnAICharacter->GetStatSystem()->InitializeCharacterStat(DataTableCharacterStat);
		}
	}

	return SpawnAICharacter;
}

void UPRAISpawnSystemComponent::CreateAIPool(FPRAIPoolSettings AIPoolSettings)
{
	if(GetWorld() != nullptr && AIPoolSettings.AICharacterClass != nullptr)
	{
		// AIPool에 추가할 AIList를 초기화하고 AI 캐릭터를 생성하여 추가합니다.
		FPRAIList AIList;
		for(int32 Index = 0; Index < AIPoolSettings.PoolSize; Index++)
		{
			APRAICharacter* SpawnAICharacter = SpawnAndInitializeAI(AIPoolSettings.AICharacterClass, Index);
			if(SpawnAICharacter)
			{
				// 초기화된 AICharacter를 AIList에 추가합니다.
				AIList.List.Emplace(SpawnAICharacter);
			}
		}

		// 초기화된 AIList를 AIPool에 추가하고 ActivateAIList를 생성합니다.
		AIPool.Emplace(AIPoolSettings.AICharacterClass, AIList);
		CreateActivateAIList(AIPoolSettings.AICharacterClass);
	}
}

void UPRAISpawnSystemComponent::CreateActivateAIList(TSubclassOf<APRAICharacter> AICharacterClass)
{
	// AICharacterClass의 ActivateAIList를 생성합니다.
	ActivateAIList.Emplace(AICharacterClass);
}

int32 UPRAISpawnSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	// 사용되지 않은 가장 작은 Index를 찾아 반환합니다.
	int32 NewIndex = 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}

void UPRAISpawnSystemComponent::OnDynamicAIDeactivate(APRAICharacter* PooledAICharacter)
{
	if(DynamicDestroyDelay > 0)
	{
		// 타이머로 딜레이 만큼 시간이 지난 후 OnDynamicAIDestroy 함수를 실행합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPRAISpawnSystemComponent::OnDynamicAIDestroy, PooledAICharacter);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// 타이머를 DynamicAIDestroyTimers에 추가합니다.
		DynamicAIDestroyTimers.Emplace(PooledAICharacter, DynamicDestroyDelayTimerHandle);
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 OnDynamicAIDestroy 함수를 실행합니다.
		OnDynamicAIDestroy(PooledAICharacter);
	}
}

void UPRAISpawnSystemComponent::OnDynamicAIDestroy(APRAICharacter* PooledAICharacter)
{
	// 사용 중인 AIListIndexes를 가지고 있는지 확인합니다.
	if(IsCreateUsedAIListIndexes(PooledAICharacter->GetClass()))
	{
		// DynamicAIDestroyTimer를 제거합니다.
		DynamicAIDestroyTimers.Remove(PooledAICharacter);
		
		// 해당 AI 캐릭터 클래스의 UsedAIListIndex를 얻습니다.
		FPRUsedAIList* NewUsedAIList = UsedAIListIndexes.Find(PooledAICharacter->GetClass());

		// 사용 중인 AIListIndex를 제거합니다.
		NewUsedAIList->Indexes.Remove(PooledAICharacter->GetAIListIndex());

		// 해당 AI 캐릭터 클래스의 AIPool이 생성되었는지 확인합니다.
		if(IsCreateAIPool(PooledAICharacter->GetClass()))
		{
			// AIPool에서 해당 AI 캐릭터를 제거하고 파괴합니다.
			AIPool.Find(PooledAICharacter->GetClass())->List.Remove(PooledAICharacter);
			PooledAICharacter->Destroy();
		}
	}
}
