// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetOnState.generated.h"

/**
 * 설정할 OnState의 종류를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPROnState : uint8
{
	OnState_OnDodge		UMETA(DisplayName = "OnDodge"),
	OnState_OnAttack	UMETA(DisplayName = "OnAttack")
};

/**
 * 설정한 OnState와 설정 값을 나타내는 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FPROnState
{
	GENERATED_BODY()

public:
	/** 설정할 OnState의 종류입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnState")
	EPROnState OnState;

	/** OnState의 설정값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnState")
	bool bIsOnState;
};

/**
 * 캐릭터의 StateSystem의 On 변수를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetOnState : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetOnState();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	void SetOnState(class APRBaseCharacter* PRCharacter, FPROnState PROnState);

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "OnState", meta = (AllowPrivateAccess = "true"))
	TArray<FPROnState> OnStates;
};
