// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Structs/Struct_PRAnimMontage.h"
#include "NiagaraSystem.h"
#include "PRDodgeSystemComponent.generated.h"

enum class EPRDirection : uint8;

/**
 * 캐릭터의 회피를 실행하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRDodgeSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRDodgeSystemComponent();

#pragma region PRDodgeSystem
public:
	/**
	 * 인자로 받은 방향에 따라 회피를 실행하는 함수입니다.
	 * @DodgeDirection: 회피하는 방향입니다.
	 */
	void ExecuteDodge(EPRDirection DodgeDirection);
#pragma endregion 

#pragma region PRAnimMontage
private:
	/**
	 * 인자로 받은 방향에 따라 회피 PRAnimMontage를 반환하는 함수입니다.
	 * @DodgeDirection: 회피하는 방향입니다.
	 */
	FPRAnimMontage GetDodgePRAnimMontage(EPRDirection DodgeDirection) const;
	
private:
	/** 현재 재생하는 회피 PRAnimMontage입니다. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage CurrentDodgePRAnimMontage;

	/** 전방 회피 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage ForwardDodgePRAnimMontage;

	/** 후방 회피 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage BackwardDodgePRAnimMontage;

	/** 좌측 회피 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage LeftDodgePRAnimMontage;

	/** 우측 회피 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage RightDodgePRAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeSystem|PRAnimMontage", Meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage AerialDodgePRAnimMontage;
#pragma endregion

#pragma region Effect
public:
	/**
	 * 입력받은 SkeletalMeshComponent에 NiagaraSystem을 스폰하여 부착하는 함수입니다.
	 * @MeshComp: NiagaraSystem을 부착할 SkeletalMeshComponent입니다.
	 * @NiagaraSystem: 스폰할 NiagaraSystem입니다.
	 */
	UNiagaraComponent* SpawnDodgeNiagaraSystem(USkeletalMeshComponent* MeshComp, UNiagaraSystem* NiagaraSystem);

private:
	/** 잔상 NiagaraSystem입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DodgeSystem|Effect", Meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DodgeAfterImageNiagaraEffect;

	/** Trail NiagaraSystem입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DodgeSystem|Effect", Meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DodgeTrailNiagaraEffect;

	/** NiagaraEffect의 색입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DodgeSystem|Effect", Meta = (AllowPrivateAccess = "true"))
	FLinearColor DodgeNiagaraEffectLinearColor;

public:
	/** DodgeAfterImageNiagaraEffect를 반환하는 함수입니다. */
	UNiagaraSystem* GetDodgeAfterImageNiagaraEffect() const;

	/** DodgeTrailNiagaraEffect를 반환하는 함수입니다. */
	UNiagaraSystem* GetDodgeTrailNiagaraEffect() const;

	/** DodgeNiagaraEffectLinearColor를 반환하는 함수입니다. */
	FLinearColor GetDodgeNiagaraEffectLinearColor() const;
#pragma endregion 
};
