// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/ActorComponent.h"
#include "Characters/PRAICharacter.h"
#include "PRAISpawnSystemComponent.generated.h"

/**
 * AIPool의 설정을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAIPoolSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRAIPoolSettings()
		: AICharacterClass(nullptr)
		, PoolSize(0)
	{}

	FPRAIPoolSettings(TSubclassOf<APRAICharacter> NewAICharacterClass, int32 NewPoolSize)
		: AICharacterClass(NewAICharacterClass)
		, PoolSize(NewPoolSize)
	{}

public:
	/** AI 캐릭터의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAIPoolSettings")
	TSubclassOf<APRAICharacter> AICharacterClass;

	/** AICharacterPool의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAIPoolSettings", meta = (ClampMin = "1"))
	int32 PoolSize;
};

/**
 * AI 캐릭터를 보관한 풀을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAIPool
{
	GENERATED_BODY()

public:
	/** AI 캐릭터를 넣은 풀입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAIPool")
	TArray<APRAICharacter*> AIs;
};

/**
 * AI 캐릭터의 정보를 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRAICharacterInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** AI 캐릭터의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPRAISpawnInfo")
	TSubclassOf<APRAICharacter> AICharacterClass;

	/** AI 캐릭터의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FPRAISpawnInfo")
	FPRCharacterStat CharacterStat;

public:
	/**
	 * 인자로 받은 PRAICharacterInfo와 같은지 판별하는 == 연산자 오버로딩입니다.
	 * 
	 * @param NewPRAICharacterInfo 비교하는 PRAICharacterInfo와 같은지 판별할 PRAICharacterInfo입니다.
	 * @return 인자로 받은 PRAICharacterInfo와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRAICharacterInfo& NewPRAICharacterInfo) const
	{
		return this->AICharacterClass == NewPRAICharacterInfo.AICharacterClass
				&& this->CharacterStat == NewPRAICharacterInfo.CharacterStat;
	}

	/**
	 * 인자로 받은 PRAICharacterInfo와 다른지 판별하는 != 연산자 오버로딩입니다.
	 * 
	 * @param NewPRAICharacterInfo 비교하는 PRAICharacterInfo와 같은지 판별할 PRAICharacterInfo입니다.
	 * @return 인자로 받은 PRAICharacterInfo와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRAICharacterInfo& NewPRAICharacterInfo) const
	{
		return this->AICharacterClass != NewPRAICharacterInfo.AICharacterClass
				|| this->CharacterStat != NewPRAICharacterInfo.CharacterStat;
	}
};

/**
 * 동적으로 생성한 AI와 AI를 제거할 때 사용하는 TimerHandle을 보관한 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRDynamicDestroyAIList
{
	GENERATED_BODY()
	
public:
	/** 동적으로 생성한 AI와 AI를 제거할 때 사용하는 TimerHandle입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicDestroyAI")
	TMap<APRAICharacter*, FTimerHandle> AIs;
};

/**
 * AI 캐릭터를 월드에 Spawn하고 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRAISpawnSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPRAISpawnSystemComponent();

protected:
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** AISpawnSystem을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void InitializeAISpawnSystem();
	
	/**
	 * 인자에 해당하는 AI 캐릭터 클래스를 AICharacterPool에서 찾아 존재할 경우 활성화하고 인자로 받은 좌표와 회전값을 적용하는 함수입니다.
	 * 
	 * @param AICharacterClass AICharacterPool에서 찾을 AI 캐릭터의 클래스 
	 * @param SpawnLocation 적용할 AI 캐릭터의 좌표
	 * @param SpawnRotation 적용할 AI 캐릭터의 회전값
	 * @return AICharacterPool에서 찾은 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	APRAICharacter* ActivateAI(TSubclassOf<APRAICharacter> AICharacterClass, FVector SpawnLocation = FVector::ZeroVector, FRotator SpawnRotation = FRotator::ZeroRotator);
	
	/** 인자에 해당하는 AI 캐릭터가 AIPool에 존재하며 활성화되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	bool IsActivateAI(APRAICharacter* PooledAICharacter) const;
	
	/** 인자에 해당하는 AI 캐릭터 클래스의 Pool이 생성되었는지 판별하는 함수입니다. */	
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	bool IsCreateAIPool(TSubclassOf<APRAICharacter> AICharacterClass) const;

	/** 인자에 해당하는 AI 캐릭터 클래스의 ActivateAIIndexList가 생성되었는지 판별하는 함수입니다. */	
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	bool IsCreateActivateAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass) const;

	/** 인자에 해당하는 AI 캐릭터 클래스의 UsedAIIndexList가 생성되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	bool IsCreateUsedAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass) const;

	/** 활성화된 AI 캐릭터들을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	TArray<APRAICharacter*> GetAllActivateAICharacters() const;

private:
	/**
	 * 인자로 받은 클래스에 해당하는 AI 캐릭터를 월드에 Spawn하는 함수입니다.
	 *
	 * @param AICharacterClass 월드에 Spawn할 AI 캐릭터의 클래스
	 * @return 월드에 Spawn한 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	APRAICharacter* SpawnAIInWorld(TSubclassOf<APRAICharacter> AICharacterClass);

	/**
	 * AI 캐릭터를 월드에 Spawn하고 초기화하는 함수입니다.
	 *
	 * @param AICharacterClass 월드에 Spawn할 AI 캐릭터의 클래스
	 * @param Index 월드에 Spawn한 AI 캐릭터의 AIList의 Index
	 * @return 월드에 Spawn한 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	APRAICharacter* SpawnAndInitializeAI(TSubclassOf<APRAICharacter> AICharacterClass, int32 Index = 0);

	/** 인자로 받은 AIPool의 설정을 바탕으로 AIPool을 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void CreateAIPool(FPRAIPoolSettings AIPoolSettings);

	/** 인자로 받은 AI 캐릭터 클래스의 ActivateAIIndexList를 생성하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void CreateActivateAIIndexList(TSubclassOf<APRAICharacter> AICharacterClass);

	/**
	 * 사용 가능한 Index를 찾아 반환하는 함수입니다.
	 *
	 * @param UsedIndexes 이미 사용 중인 Index 목록
	 * @return 사용 가능한 Index
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	int32 FindAvailableIndex(const TSet<int32>& UsedIndexes);

	/**
	 * 인자로 받은 AI 캐릭터를 비활성화하는 함수입니다.
	 * AI 캐릭터의 OnAIDeactivate 델리게이트 바인딩합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void OnAIDeactivate(APRAICharacter* PooledAICharacter);
	
	/**
	 * 동적으로 생성한 AI 캐릭터를 비활성화 하는 함수입니다.
	 * AI 캐릭터의 OnDynamicAIDeactivate 델리게이트에 바인딩합니다.
	 *
	 * @param PooledAICharacter 비활성화할 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void OnDynamicAIDeactivate(APRAICharacter* PooledAICharacter);

	/**
	 * 동적으로 생성한 AI 캐릭터를 제거하는 함수입니다.
	 *
	 * @param PooledAICharacter 제거할 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAISpawnSystem")
	void OnDynamicAIDestroy(APRAICharacter* PooledAICharacter);

private:
	/** AIPool의 설정 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true"))
	UDataTable* AIPoolSettingsDataTable;
	
	/** 월드에 Spawn한 AI의 Pool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRAICharacter>, FPRAIPool> AIPool;

	/** 활성화된 AI의 Index의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRAICharacter>, FPRActivateIndexList> ActivateAIIndexList;

	/**
	 * AI 캐릭터를 PoolSize보다 많이 동적으로 생성할 때 사용한 AI 캐릭터의 Index의 목록입니다.
	 * 동적으로 생성하는 AI 캐릭터의 Index에 오류가 생기지 않도록 하기 위해서 사용합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRAICharacter>, FPRUsedIndexList> UsedAIIndexList;

	/** 동적으로 제거할 AI 캐릭터의 목록입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<APRAICharacter>, FPRDynamicDestroyAIList> DynamicDestroyAIList;

	/** 동적으로 생성한 AI 캐릭터를 제거하는 딜레이 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAISpawnSystem", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	float DynamicDestroyDelay;
};
