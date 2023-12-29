// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/ActorComponent.h"
#include "PRBaseActorComponent.generated.h"

class APRBaseCharacter;

/**
 * PRBaseCharacter가 사용하는 ActorComponent의 기본이 되는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRBaseActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPRBaseActorComponent();

protected:
	/** 액터 컴포넌트가 액터에 등록될 때 호출하는 함수입니다. */
	virtual void OnRegister() override;
	
protected:
	/** PROwner를 초기화하는 함수입니다. */
	void InitializePROwner();

private:
	/** 이 ActorComponent를 소유하고 있는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacterReference", meta = (AllowPrivateAccess = "true"))
	APRBaseCharacter* PROwner;

public:
	/** PROwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetPROwner() const;
};
