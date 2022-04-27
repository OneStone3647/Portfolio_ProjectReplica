// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/ActorComponent.h"
#include "PRBaseActorComponent.generated.h"

/**
 * PROwner를 사용하는 ActorComponent의 베이스가 되는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRBaseActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPRBaseActorComponent();

protected:
	virtual void BeginPlay() override;

#pragma region PRCharacterReference
protected:
	/** PROwner를 초기화하는 함수입니다. */
	void InitializePROwner();
	
protected:
	/** 이 ActorComponent를 소유하는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseActorComponent|PRCharacterReference")
	class APRBaseCharacter* PROwner;
#pragma endregion
};
