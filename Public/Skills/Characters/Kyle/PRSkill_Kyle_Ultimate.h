// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRCooldownSkill.h"
#include "PRSkill_Kyle_Ultimate.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Kyle의 궁극기 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_Kyle_Ultimate : public UPRCooldownSkill
{
	GENERATED_BODY()

public:
	UPRSkill_Kyle_Ultimate();

public:
	/** 스킬을 초기화하는 함수입니다. */
	virtual void InitializeSkill_Implementation() override;
	
	/** 스킬을 실행하는 함수입니다. */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;

#pragma region GeneralUltimate
private:
	/** GeneralUltimate를 시전하는 함수입니다. */
	void CastingGeneralUltimate();
	
	/** GeneralUltimate를 실행하는 함수입니다. */
	UFUNCTION()
	void ActivateGeneralUltimate();

	/** GeneralUltimate를 마무리하는 함수입니다.*/
	UFUNCTION()
	void FinishGeneralUltimate();

	/**
	 * 입력받은 인자 값에 따라 플레이어의 Visibility를 설정하는 함수입니다.
	 *
	 * @param bVisibility true일 경우 플레이어를 숨기고 false일 경우 플레이어를 나타냅니다.
	 */
	void SetSkillOwnerVisibility(bool bVisibility);
	
private:
	/** GeneralUltimate를 시전하는 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage CastingGeneralUltimatePRAnimMontage;

	/** GeneralUltimate를 마무리하는 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage FinishGeneralUltimatePRAnimMontage;	
	
	/** GeneralUltimate의 오브젝트 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	FName GeneralUltimateObjectName;

	/** GeneralUltimate의 시전 시간입니다. 시전 시간이 지난 후 GeneralUltimate를 실행합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	float GeneralUltimateCastingTime;

	/** GeneralUltimate의 마무리 시간입니다. GeneralUltimate 실행 후 시간이 지나면 FinishGeneralUltimate를 실행합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	float GeneralUltimateFinishTime;

	/** GeneralUltimate의 실행에 사용하는 TimerHandle입니다. */
	FTimerHandle ActivateGeneralUltimateTimerHandle;

	/** GeneralUltimate의 마무리에 사용하는 TimerHandle입니다. */
	FTimerHandle FinishGeneralUltimateTimerHandle;
#pragma endregion

#pragma region AwakeningUltimate

#pragma endregion
};
