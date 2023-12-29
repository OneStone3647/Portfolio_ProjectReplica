// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectReplicaGameMode.h"
#include "Components/PRAISpawnSystemComponent.h"
#include "Components/PRTimeStopSystemComponent.h"

AProjectReplicaGameMode::AProjectReplicaGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }

	// AISpawnSystem
	AISpawnSystem = CreateDefaultSubobject<UPRAISpawnSystemComponent>(TEXT("AISpawnSystem"));

	// TimeStopSystem
	TimeStopSystem = CreateDefaultSubobject<UPRTimeStopSystemComponent>(TEXT("TimeStopSystem"));
}

void AProjectReplicaGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// AISpawnSystem을 초기화하여 초기 AIPool을 생성합니다.
	GetAISpawnSystem()->InitializeAISpawnSystem();
}

#pragma region AISpawnSystem
APRAICharacter* AProjectReplicaGameMode::ActivateAI(TSubclassOf<APRAICharacter> AICharacterClass, FVector SpawnLocation, FRotator SpawnRotation)
{
	return GetAISpawnSystem()->ActivateAI(AICharacterClass, SpawnLocation, SpawnRotation);
}
#pragma endregion 

#pragma region TimeStopSystem
bool AProjectReplicaGameMode::IsActivateTimeStop() const
{
	return GetTimeStopSystem()->IsActivateTimeStop();
}

void AProjectReplicaGameMode::ActivateTimeStop(float TimeStopDuration)
{
	GetTimeStopSystem()->ActivateTimeStop(TimeStopDuration);
}

void AProjectReplicaGameMode::DeactivateTimeStop()
{
	GetTimeStopSystem()->DeactivateTimeStop();
}
#pragma endregion 
