// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PRBaseAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PRBaseCharacter.h"

APRBaseAIController::APRBaseAIController()
{
	BehaviorTreeAsset = nullptr;
	BlackboardAsset = nullptr;
}

void APRBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	StartAI();
}

#pragma region AI
void APRBaseAIController::StartAI()
{
	// AI가 BlackboardAsset을 사용합니다.
	if(UseBlackboard(BlackboardAsset, Blackboard))
	{
		// AI가 BehaviorTreeAsset을 사용합니다.
		if(RunBehaviorTree(BehaviorTreeAsset))
		{
			// Blackboard의 Key 값을 설정합니다.
			// 딜레이를 줘야 GetPlayerCharacter가 정상적으로 실행됨
			GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		}
		else
		{
			PR_LOG_ERROR("AIController couldn't run BehaviorTree");
		}
	}
}

void APRBaseAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BehaviorTreeComponent != nullptr)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
#pragma endregion 
