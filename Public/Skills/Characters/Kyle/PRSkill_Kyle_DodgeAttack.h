// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRNoneCooldownSkill.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "PRSkill_Kyle_DodgeAttack.generated.h"

/**
 * 캐릭터가 회피 후 일반공격을 입력할 시 필요 게이지를 소모하여 참격을 날리는 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_Kyle_DodgeAttack : public UPRNoneCooldownSkill
{
	GENERATED_BODY()

public:
	UPRSkill_Kyle_DodgeAttack();
	
public:
	/** 스킬을 초기화하는 함수입니다. */
	virtual void InitializeSkill_Implementation() override;
	
	/** 스킬을 실행하는 함수입니다. */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;

public:
	/** 입력받은 인자로 bActivateSkill을 설정하는 함수입니다. */
	virtual void SetActivateSkill(bool bNewActivateSkill) override;

protected:
	/** 지속효과가 종료되었을 때 실행하는 함수입니다. */
	virtual void EndDurationEffect() override;
	
private:
	/** 회피 공격에 사용하여 대미지를 가하는 오브젝트의 정보입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectInfo", meta = (AllowPrivateAccess = "true"))
	FPRPooledObjectInfo DodgeAttackProjectile;
	
	/** 회피 공격 자세의 지속시간에 사용하는 TimerHandle입니다. */
	FTimerHandle SkillTimerHandle;

	/** 회피 공격 자세의 지속시간입니다. 지속시간이 지나면 회피 공격을 할 수 없습니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeAttack", meta = (AllowPrivateAccess = "true"))
	float SkillDurationTime;

	/** 스킬 게이지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeAttack", meta = (AllowPrivateAccess = "true"))
	float SkillGauge;

	/** 스킬 게이지 소모값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeAttack", meta = (AllowPrivateAccess = "true"))
	float SkillGaugeCost;	

	/** 스킬 게이지의 최소값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeAttack", meta = (AllowPrivateAccess = "true"))
	float MinSkillGauge;

	/** 스킬 게이지의 최대값입니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeAttack", meta = (AllowPrivateAccess = "true"))
	float MaxSkillGauge;

#pragma region PRAnimMontage
private:
	/** 스킬 실행시 사용하는 PRComboAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage", meta = (AllowPrivateAccess = "true"))
	FPRComboAnimMontage PRComboAnimMontage;

	/** PRComboAnimMontage의 시작 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRAnimMontage", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 PRComboAnimMontageFromID;

	/** PRComboAnimMontage의 끝 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRAnimMontage", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 PRComboAnimMontageToID;
#pragma endregion
};
