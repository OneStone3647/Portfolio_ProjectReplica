// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRAISpawner.generated.h"

class APRAICharacter;
class USphereComponent;
class UArrowComponent;

/**
 * GameInstance에서 AI의 정보를 가진 데이터 테이블을 가져와 이 Actor의 현재 위치에 Spawn하는 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APRAISpawner();

protected:
	virtual void BeginPlay() override;

public:
	/** SpawnInfo에 해당하는 AI를 AISpawner의 현재 위치에 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "AISpawner")
	void SpawnAI();

private:
	/** Spawner의 중심이 될 SphereComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawner", meta = (AllowPrivateAccess = "true"))
	USphereComponent* RootSphere;

	/** Spawn된 AI가 바라보는 방향을 나타내는 ArrowComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawner", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* AIDirectionArrow;	
	
	// /** Spawn할 AI의 ID입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner", meta = (AllowPrivateAccess = "true"))
	// int32 AIID;

	/** Spawn할 AI 캐릭터의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APRAICharacter> SpawnAICharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AISpawner", meta = (AllowPrivateAccess = "true"))
	APRAICharacter* SpawnAICharacter;
};
