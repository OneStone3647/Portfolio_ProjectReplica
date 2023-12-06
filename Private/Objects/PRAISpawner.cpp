// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRAISpawner.h"
#include "ProjectReplicaGameInstance.h"
#include "ProjectReplicaGameMode.h"
#include "Characters/PRAICharacter.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/PRAISpawnSystemComponent.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"

APRAISpawner::APRAISpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// RootSphere
	RootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphere"));
	RootSphere->SetCollisionProfileName(TEXT("NoCollision"));
	RootSphere->SetSphereRadius(64.0f);
	SetRootComponent(RootSphere);

	// AIDirectionArrow
	AIDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("AIDirectionArrow"));
	AIDirectionArrow->SetupAttachment(RootComponent);

	// AISpawnInfo
	// AIID = 0;
	SpawnAICharacterClass = nullptr;
	SpawnAICharacter = nullptr;
}

void APRAISpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnAI();
}

void APRAISpawner::SpawnAI()
{
	// UProjectReplicaGameInstance* PRGameInstance = Cast<UProjectReplicaGameInstance>(GetGameInstance());
	// if(PRGameInstance != nullptr)
	// {
	// 	FPRAISpawnInfo NewAISpawnInfo = PRGameInstance->GetAISpawnInfoFromDataTable(AIID);
	// 	if(NewAISpawnInfo != FPRAISpawnInfo())
	// 	{
	// 		APRAICharacter* SpawnPRAICharacter = SpawnAIInWorld(NewAISpawnInfo.AICharacterClass);
	// 		if(SpawnPRAICharacter != nullptr)
	// 		{
	// 			// GameMode의 TimeStopSystem에 AI캐릭터를 등록합니다.
	// 			
	// 			// 기본 컨트롤러를 Spawn합니다.
	// 			SpawnPRAICharacter->SpawnDefaultController();
	// 			SpawnPRAICharacter->SetSpawner(this);
	// 			// Spawner의 위치와 회전값으로 Spawn한 AI의 위치와 회전값을 설정합니다.
	// 			SpawnPRAICharacter->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
	// 		}
	// 	}
	// }

	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(PRGameMode != nullptr)
	{
		UPRAISpawnSystemComponent* AISpawnSystem = PRGameMode->GetAISpawnSystem();
		// APRAICharacter* SpawnAICharacter = AISpawnSystem->ActivateAI(SpawnAICharacterClass, GetActorLocation(), GetActorRotation());
		SpawnAICharacter = AISpawnSystem->ActivateAI(SpawnAICharacterClass, GetActorLocation(), GetActorRotation());
		if(SpawnAICharacter != nullptr)
		{
			SpawnAICharacter->SetSpawner(this);
		}
	}
}

