// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/GameInstance.h"
#include "ProjectReplicaGameInstance.generated.h"

/**
 * 게임의 인스턴스 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UProjectReplicaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UProjectReplicaGameInstance();

#pragma region AIInfoDataTable
private:
	/** AI 캐릭터의 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AIInfoDataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* AIInfoDataTable;

public:
	/** AIInfoDataTable을 반환하는 함수입니다. */
	UDataTable* GetAIInfoDataTable() const;
#pragma endregion 
};
