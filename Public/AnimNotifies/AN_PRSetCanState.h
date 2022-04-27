// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetCanState.generated.h"

/**
 * 설정할 CanState의 종류를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCanState : uint8
{
	CanState_CanMove			UMETA(DisplayName = "CanMove"),
	CanState_CanDodge			UMETA(DisplayName = "CanDodge"),
	CanState_CanSwapWeapon		UMETA(DisplayName = "CanSwapWeapon")
};

/**
 * 설정한 CanState와 설정 값을 나타내는 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FPRCanState
{
	GENERATED_BODY()

public:
	/** 설정할 CanState의 종류입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CanState")
	EPRCanState CanState;

	/** CanState의 설정값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CanState")
	bool bIsCanState;
};

/**
 * 캐릭터의 StateSystem의 Can 변수를 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetCanState : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetCanState();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;
	
private:
	void SetCanState(class APRBaseCharacter* PRCharacter, FPRCanState PRCanState);

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CanState", meta = (AllowPrivateAccess = "true"))
	TArray<FPRCanState> CanStates;
};
