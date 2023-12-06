// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Skills/PRCooldownSkill.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "PRSkill_Kyle_GeneralUltimate.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class APRScreenShatter;

/**
 * 캐릭터가 일반 상태일 때 사용하는 궁극기 스킬 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRSkill_Kyle_GeneralUltimate : public UPRCooldownSkill
{
	GENERATED_BODY()
	
public:
	UPRSkill_Kyle_GeneralUltimate();

public:
	/** 스킬을 초기화하는 함수입니다. */
	virtual void InitializeSkill_Implementation() override;
	
	/** 스킬을 실행하는 함수입니다. */
	virtual bool ActivateSkill_Implementation() override;

	/** 스킬을 실행할 수 있는지판별하는 함수입니다. */
	virtual bool IsCanActivateSkill_Implementation() const override;

	/**
	 * AN_PROnSkill 클래스의 Notify 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간에서 실행하고 싶을 때 사용합니다.
	 */
	virtual bool OnSkillNotify_Implementation() override;

	/**
	 * ANS_PROnSkill 클래스의 NotifyTick 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간을 지나는 동안 실행하고 싶을 때 사용합니다.
	 */
	virtual bool OnSkillNotifyTick_Implementation(float DeltaTime) override;

#pragma region Debug
private:
	/** Debug 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool bActivateDebug;
#pragma endregion
	
private:
	/** Ultimate를 시전하는 함수입니다. */
	void CastingUltimate();
	
	/** Ultimate를 실행하는 함수입니다. */
	void ActivateUltimate();

	/** Ultimate를 마무리하는 함수입니다. */
	void FinishUltimate();

protected:
	/** 입력받은 인자로 플레이어 캐릭터와 Weapon의 Visibility를 설정하는 함수입니다. */
	void SetVisibilityPlayerCharacterAndWeapon(bool bNewVisibility);

	/** UltimateSlashEffect와 UltimateMirrorObject를 스폰하고 범위 안에 존재하는 대상에게 대미지를 입히는 함수입니다. OnSkillAnimNotify 함수에서 실행됩니다. */
	void ActivateUltimateSlash();

	/** UltimateSlashEffect와 UltimateMirrorObject를 비활성화하는 함수입니다. */
	void DeactivateUltimateSlash();

	/** Ultimate의 공격 범위에 존재하는 액터들에게 입력받은 인자만큼 대미지를 주는 함수입니다. */
	void ApplyDamageUltimateArea(float NewApplyDamage);

public:
	/** 화면 깨짐 효과를 실행하는함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "GeneralUltimate")
	void ActivateScreenShatter();
	
private:
	/** Ultimate의 공격 범위입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	FVector UltimateAreaExtent;

	/** Ultimate의 참격 대미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	float UltimateSlashDamage;

	/** Ultimate의 마무리 대미지 입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	float UltimateFinishDamage;
	
	/** Ultimate를 시전했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	bool bCastingUltimate;	
	
	/** GeneralUltimate를 실행했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	bool bActivateUltimate;

	/** ActivateUltimate 함수를 실행한 횟수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	int32 ActivateUltimateCount;

	/** Ultimate의 마무리를 나타내는 변수입니다. */	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	bool bFinishUltimate;

	/** FinishUltimate 함수를 실행한 횟수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	int32 FinishUltimateCount;

	/** Ultimate을 실행한 위치입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate", meta = (AllowPrivateAccess = "true"))
	FVector UltimateActivateLocation;
	
	/** Ultimate를 시전하는 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|Animation", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage CastingUltimatePRAnimMontage;

	/** Ultimate를 마무리하는 PRAnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|Animation", meta = (AllowPrivateAccess = "true"))
	FPRAnimMontage FinishUltimatePRAnimMontage;

	/** Ultimate를 시전하는 동안 재생하는 Sound입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* UltimateCastingSound;

	/** Ultimate의 참격 Sound입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* UltimateSlashSound;

	/** UltimateCastingSound를 재생하는 AudioComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate|Sound", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* UltimateCastingAudioComponent;

	/** Ultimate의 참격을 실행할 때 주변을 반사하는 오브젝트입니다. */	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	FPRPooledObjectInfo UltimateSlashMirrorObject;

	/** Ultimate의 참격 Effect입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	FPRNiagaraEffectInfo UltimateSlashNiagaraEffect;

	/** UltimateSlashNiagaraEffect를 Spawn하는 범위입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	FVector UltimateSlashSpawnAreaExtent;

	/** Ultimate의 참격을 Spawn하는 간격입니다. */	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	int32 UltimateSlashSpawnInterval;
	
	/** Ultimate의 참격을 실행하는 간격입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	float UltimateSlashInterval;

	/** Ultimate의 참격을 실행하는 간격의 누적 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GeneralUltimate|Effect", meta = (AllowPrivateAccess = "true"))
	float UltimateSlashAccumulatedInterval;

	/** 캡쳐한 화면의 깨짐을 표현하는 ScreenShatter 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|ScreenShatter", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APRScreenShatter> ScreenShatter;

	/** 캡쳐한 화면을 나타내는 위젯 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralUltimate|ScreenShatter", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ScreenShatterWidget;
	
public:
	/** 입력받은 인자로 UltimateSlashInterval를 설정하는 함수입니다. */
	void SetUltimateSlashInterval(float NewSlashInterval);
	
	/** 입력받은 인자로 UltimateSlashAccumulatedInterval를 설정하는 함수입니다. */
	void SetUltimateSlashAccumulatedInterval(float NewSlashAccumulatedInterval);
};
