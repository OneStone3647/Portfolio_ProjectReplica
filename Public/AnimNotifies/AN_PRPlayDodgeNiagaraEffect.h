// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRPlayDodgeNiagaraEffect.generated.h"

/**
 * 실행할 NiagaraEffect를 구분하는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRDodgeNiagaraEffect : uint8
{
	DodgeNiagaraEffect_Trail			UMETA(DisplayName = "Trail"),
	DodgeNiagaraEffect_AfterImage		UMETA(DisplayName = "AfterImage")
};

/**
 * 회피를 할 때 출력할 NiagaraEffect를 실행하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRPlayDodgeNiagaraEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRPlayDodgeNiagaraEffect();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	/** 실행할 NiagaraEffect를 구분하는 열거형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDodgeNiagaraEffect", Meta = (AllowPrivateAccess = "true"))
	EPRDodgeNiagaraEffect DodgeNiagaraEffect;
	
	/** NiagaraEffect의 루프 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDodgeNiagaraEffect", Meta = (AllowPrivateAccess = "true"))
	float LoopDuration;
};
