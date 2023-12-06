// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectReplicaGameMode.generated.h"

class UPRAISpawnSystemComponent;
class UPRTimeStopSystemComponent;

UCLASS(minimalapi)
class AProjectReplicaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectReplicaGameMode();

protected:
	virtual void PostInitializeComponents() override;

#pragma region AISpawnSystem
public:
	/**
	 * 인자에 해당하는 AI 캐릭터 클래스를 AIPool에서 찾아 존재할 경우 활성화하고 인자로 받은 좌표와 회전값을 적용하는 함수입니다.
	 * 
	 * @param AICharacterClass AIPool에서 찾을 AI 캐릭터의 클래스 
	 * @param SpawnLocation 적용할 AI 캐릭터의 좌표
	 * @param SpawnRotation 적용할 AI 캐릭터의 회전값
	 * @return AIPool에서 찾은 AI 캐릭터
	 */
	UFUNCTION(BlueprintCallable, Category = "AISpawnSystem")
	APRAICharacter* ActivateAI(TSubclassOf<APRAICharacter> AICharacterClass, FVector SpawnLocation = FVector::ZeroVector, FRotator SpawnRotation = FRotator::ZeroRotator);
	
private:
	/** AI를 월드에 Spawn하고 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawnSystem", meta = (AllowPrivateAccess = "true"))
	UPRAISpawnSystemComponent* AISpawnSystem;

public:
	/** AISpawnSystem을 반환하는 함수입니다. */
	class UPRAISpawnSystemComponent* GetAISpawnSystem() const { return AISpawnSystem; }
#pragma endregion 

#pragma region TimeStopSystem
public:
	/** TimeStop이 실행 중인지 나타내는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStopSystem")
	bool IsActivateTimeStop() const;

	/** TimeStop을 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStopSystem")
	void ActivateTimeStop(float TimeStopDuration);

	/** TimeStop을 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStopSystem")
	void DeactivateTimeStop();
	
private:
	/** 플레이어 캐릭터를 제외한 모든 오브젝트들이 시간이 멈춘것처럼 일시정지 시키는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TimeStopSystem", meta = (AllowPrivateAccess = "true"))
	UPRTimeStopSystemComponent* TimeStopSystem;

public:
	/** TimeStopSystem을 반환하는 함수입니다. */
	class UPRTimeStopSystemComponent* GetTimeStopSystem() const { return TimeStopSystem; }
#pragma endregion
};



