// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRTimedNiagaraEffect.generated.h"

class UNiagaraSystem;

/**
 * NiagaraEffect를 일정 시간 활성화하고 종료시 비활성화하는 AnimNotifyState 클래스입니다.
 * NotifyBegin에서 NiagaraEffect를 활성화하고 NotifyEnd에서 비활성화합니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRTimedNiagaraEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRTimedNiagaraEffect();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** Template를 바탕으로 하는 NiagaraComponent를 생성하고 MeshComp에 부착하는 함수입니다. */
	class UNiagaraComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;

	/** 셍성하여 Mesh에 부착한 NiagaraComponent를 반환하는 함수입니다. */
	class UNiagaraComponent* GetSpawnedEffect(UMeshComponent* MeshComp);

private:
	/** 생성할 NiagaraSystem입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* Template;

	/** 생성한 NiagaraComponent를 부착할 Mesh의 소켓이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	/** 생성하는 NiagaraComponent의 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FVector LocationOffset;

	/** 생성하는 NiagaraComponent의 회전 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FRotator RotationOffset;

	/** NiagaraComponent의 색입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	FLinearColor EffectColor;

	/** NiagaraComponent의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	float EffectSize;

	/** NotifyEnd에서 NiagaraComponent를 제거할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTimedNiagaraEffect", meta = (AllowPrivateAccess = "true"))
	bool bDestroyAtEnd;
};
