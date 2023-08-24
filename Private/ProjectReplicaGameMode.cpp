// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectReplicaGameMode.h"
#include "ProjectReplica/ProjectReplicaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectReplicaGameMode::AProjectReplicaGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
}
