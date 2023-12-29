// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRAISpawnSystemComponent.h"
#include "Components/PRStatSystemComponent.h"
#include "ProjectReplicaGameInstance.h"
#include "Kismet/GameplayStatics.h"

UPRAISpawnSystemComponent::UPRAISpawnSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AIPoolSettingsDataTable = nullptr;
	
	// AIPool과 ActivateAIIndexList, UsedAIIndexList, DynamicDestroyAIList를 초기화합니다.
	AIPool.Empty();
	ActivateAIIndexList.Empty();
	UsedAIIndexList.Empty();
	DynamicDestroyAIList.Empty();

	// 동적으로 생성한 AI 캐릭터를 제거하는 딜레이 시간을 초기화합니다.
	DynamicDestroyDelay = 60.0f;
}

void UPRAISpawnSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// 모든 AIPool을 제거합니다.
	if(AIPool.Num() > 0)
	{
		for(auto& Pair : AIPool)
		{
			for(auto& AICharacter : Pair.Value.AIs)
			{
				AICharacter->Destroy();
			}
		}

		AIPool.Empty();
	}
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPRAISpawnSystemComponent::InitializeAISpawnSystem()
{
	AIPool.Empty();
	ActivateAIIndexList.Empty();
	UsedAIIndexList.Empty();
	DynamicDestroyAIList.Empty();
	
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

	// AIPool에서 해당 AI 캐릭터 클래스의 풀을 얻습니다.
	FPRAIPool* Pair = AIPool.Find(AICharacterClass);
	if(Pair == nullptr)
	{
		// 지정된 AI 캐릭터 클래스가 없습니다.
		return nullptr;
	}

	APRAICharacter* ActivateableAICharacter = nullptr;

	// AIList에서 활성화 되지 않은 AI 캐릭터를 찾아 활성화합니다.
	for(const auto& AICharacter : Pair->AIs)
	{
		if(IsValid(AICharacter) && !IsActivateAI(AICharacter))
		{
			ActivateableAICharacter = AICharacter;
			break;
		}
	}

	// Pair의 모든 AI 캐릭터가 활성화 되었을 경우 새로운 AI 캐릭터를 생성합니다.
	if(ActivateableAICharacter == nullptr)
	{
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 AI 캐릭터 클래스의 UsedAIIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedAIIndexList(AICharacterClass))
		{
			FPRUsedIndexList NewUsedAIIndexList;
			for(const auto& AICharacter : Pair->AIs)
			{
				NewUsedAIIndexList.Indexes.Add(AICharacter->GetPoolIndex());
			}
			
			UsedAIIndexList.Emplace(AICharacterClass, NewUsedAIIndexList);
		}

		// UsedAIIndexList에서 해당 AI 캐릭터 클래스가 사용중인 AIListIndex 목록을 얻습니다.
		FPRUsedIndexList* NewUsedAIIndexList = UsedAIIndexList.Find(AICharacterClass);
		if(NewUsedAIIndexList == nullptr)
		{
			// 지정된 AI 캐릭터 클래스가 없습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(NewUsedAIIndexList->Indexes);

		// 사용 가능한 Index를 사용중인 UsedAIIndexList에 추가합니다.
		NewUsedAIIndexList->Indexes.Add(NewIndex);

		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 AI 캐릭터를 생성하고 초기화하며 사용 가능한 Index를 설정합니다.
		ActivateableAICharacter = SpawnAndInitializeAI(AICharacterClass, NewIndex);
		ActivateableAICharacter->OnDynamicAIDeactivate.AddDynamic(this, &UPRAISpawnSystemComponent::OnDynamicAIDeactivate);

		// 새로 생성한 AI 캐릭터를 Pair에 추가합니다.
		Pair->AIs.Emplace(ActivateableAICharacter);
	}

	// DynamicAIDestroyTimer를 정지합니다.
	FPRDynamicDestroyAIList* DynamicDestroyPair = DynamicDestroyAIList.Find(ActivateableAICharacter->GetClass());
	if(DynamicDestroyPair != nullptr)
	{
		FTimerHandle* DynamicAIDestroyTimer = DynamicDestroyPair->AIs.Find(ActivateableAICharacter);
		if(DynamicAIDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicAIDestroyTimer);
		}
	}

	// AI 캐릭터를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
	ActivateableAICharacter->Activate();
	ActivateableAICharacter->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	
	// 해당 AI 캐릭터 클래스로 처음 활성화하는 경우 ActivateAIIndexList를 생성합니다.
	if(!IsCreateActivateAIIndexList(AICharacterClass))
	{
		CreateActivateAIIndexList(AICharacterClass);
	}
	
	// 활성화된 AI 캐릭터의 PoolIndex를 ActivateAIIndexList에 추가합니다.
	ActivateAIIndexList.Find(AICharacterClass)->Indexes.AddUnique(ActivateableAICharacter->GetPoolIndex());

	return ActivateableAICharacter;
}

bool UPRAISpawnSystemComponent::IsActivateAI(APRAICharacter* PooledAICharacter) const
{
	const FPRActivateIndexList* NewActivateAIIndexList = ActivateAIIndexList.Find(PooledAICharacter->GetClass());
	if(NewActivateAIIndexList != nullptr)
	{
		return NewActivateAIIndexList->Indexes.Contains(PooledAICharacter->GetPoolIndex()) && PooledAICharacter->IsActivate();
	}

	return false;
}

bool UPRAISpawnSystemComponent::IsCreateAIPool(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return AIPool.Contains(AICharacterClass);
}

bool UPRAISpawnSystemComponent::IsCreateActivateAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return ActivateAIIndexList.Contains(AICharacterClass);
}

bool UPRAISpawnSystemComponent::IsCreateUsedAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass) const
{
	return UsedAIIndexList.Contains(AICharacterClass);
}

TArray<APRAICharacter*> UPRAISpawnSystemComponent::GetAllActivateAICharacters() const
{
	TArray<APRAICharacter*> ActivateAICharacters;

	// AIPool의 모든 항목을 순회합니다.
	for(const auto& Pair : AIPool)
	{
		// 현재 AI 캐릭터의 클래스의 목록을 얻습니다.
		const TSubclassOf<APRAICharacter> AICharacterClass = Pair.Key;
		const FPRAIPool AICharacterPool = Pair.Value;

		// 현재 AI 캐릭터 클래스에 대한 활성화된 Index 목록을 얻습니다.
		const FPRActivateIndexList* NewActivateAIIndexList = ActivateAIIndexList.Find(AICharacterClass);
		if(NewActivateAIIndexList != nullptr && NewActivateAIIndexList->Indexes.Num() > 0)
		{
			// NewActivateAIIndexList에 있는 ListIndex에 해당하는 AICharacter를 ActivateAICharacters에 추가합니다.
			for(int32 ListIndex : NewActivateAIIndexList->Indexes)
			{
				for(const auto& AICharacter : AICharacterPool.AIs)
				{
					if(AICharacter->GetPoolIndex() == ListIndex)
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
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		return GetWorld()->SpawnActor<APRAICharacter>(AICharacterClass, ActorSpawnParameters);
		
		// return GetWorld()->SpawnActor<APRAICharacter>(AICharacterClass);
	}

	return nullptr;
}

APRAICharacter* UPRAISpawnSystemComponent::SpawnAndInitializeAI(TSubclassOf<APRAICharacter> AICharacterClass, int32 Index)
{
	APRAICharacter* SpawnAICharacter = SpawnAIInWorld(AICharacterClass);
	if(SpawnAICharacter != nullptr)
	{
		// 기본 AI 컨트롤러를 Spawn합니다.
		// 동적으로 생성한 AI 캐릭터는 AI 컨트롤러를 직접 Spawn해야 캐릭터가 중력의 영향을 받습니다.
		SpawnAICharacter->SpawnDefaultController();
		
		// 생성된 AI 캐릭터를 비활성화하고, Index를 설정하며, OnAIDeactivate를 바인딩합니다.
		SpawnAICharacter->Deactivate();
		SpawnAICharacter->SetPoolIndex(Index);
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
		// AIPool에 추가할 Pair를 초기화하고 AI 캐릭터를 생성하여 추가합니다.
		FPRAIPool Pair;
		for(int32 Index = 0; Index < AIPoolSettings.PoolSize; Index++)
		{
			APRAICharacter* SpawnAICharacter = SpawnAndInitializeAI(AIPoolSettings.AICharacterClass, Index);
			if(SpawnAICharacter != nullptr)
			{
				// 초기화된 AICharacter를 Pair에 추가합니다.
				Pair.AIs.Emplace(SpawnAICharacter);
			}
		}

		// 초기화된 Pair를 AIPool에 추가하고 ActivateAIIndexList를 생성합니다.
		AIPool.Emplace(AIPoolSettings.AICharacterClass, Pair);
		CreateActivateAIIndexList(AIPoolSettings.AICharacterClass);
	}
}

void UPRAISpawnSystemComponent::CreateActivateAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass)
{
	// AICharacterClass의 ActivateAIIndexList를 생성합니다.
	ActivateAIIndexList.Emplace(AICharacterClass);
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

void UPRAISpawnSystemComponent::OnAIDeactivate(APRAICharacter* PooledAICharacter)
{
	if(IsActivateAI(PooledAICharacter))
	{
		// 비활성화된 AI 캐릭터의 Index를 ActivateAIIndexList에서 제거합니다.
		ActivateAIIndexList.Find(PooledAICharacter->GetClass())->Indexes.Remove(PooledAICharacter->GetPoolIndex());
	}
}

void UPRAISpawnSystemComponent::OnDynamicAIDeactivate(APRAICharacter* PooledAICharacter)
{
	if(DynamicDestroyDelay > 0)
	{
		// 타이머로 딜레이 만큼 시간이 지난 후 OnDynamicAIDestroy 함수를 실행합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPRAISpawnSystemComponent::OnDynamicAIDestroy, PooledAICharacter);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// 타이머를 DynamicDestroyAIList에 추가합니다.
		FPRDynamicDestroyAIList* DynamicDestroyPair = DynamicDestroyAIList.Find(PooledAICharacter->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->AIs.Emplace(PooledAICharacter, DynamicDestroyDelayTimerHandle);
		}
		else
		{
			FPRDynamicDestroyAIList AIList;
			AIList.AIs.Emplace(PooledAICharacter, DynamicDestroyDelayTimerHandle);
			DynamicDestroyAIList.Emplace(PooledAICharacter->GetClass(), AIList);
		}
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 OnDynamicAIDestroy 함수를 실행합니다.
		OnDynamicAIDestroy(PooledAICharacter);
	}
}

void UPRAISpawnSystemComponent::OnDynamicAIDestroy(APRAICharacter* PooledAICharacter)
{
	// 사용 중인 UsedAIIndexList를 가지고 있는지 확인합니다.
	if(IsCreateUsedAIIndexList(PooledAICharacter->GetClass()))
	{
		// DynamicAIDestroyTimer를 제거합니다.
		FPRDynamicDestroyAIList* DynamicDestroyPair = DynamicDestroyAIList.Find(PooledAICharacter->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->AIs.Remove(PooledAICharacter);
		}
		
		// 해당 AI 캐릭터 클래스의 UsedAIIndexList를 얻습니다.
		FPRUsedIndexList* NewUsedIndexList = UsedAIIndexList.Find(PooledAICharacter->GetClass());
		if(NewUsedIndexList != nullptr)
		{
			// 사용 중인 Index를 제거합니다.
			NewUsedIndexList->Indexes.Remove(PooledAICharacter->GetPoolIndex());
		}

		// 해당 AI 캐릭터 클래스의 AIPool이 생성되었는지 확인합니다.
		if(IsCreateAIPool(PooledAICharacter->GetClass()))
		{
			// AIPool에서 해당 AI 캐릭터를 제거하고 파괴합니다.
			AIPool.Find(PooledAICharacter->GetClass())->AIs.Remove(PooledAICharacter);
			PooledAICharacter->Destroy();
		}
	}
}
