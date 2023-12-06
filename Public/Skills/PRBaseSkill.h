// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "MovieSceneSequencePlayer.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "PRBaseSkill.generated.h"

class APRBaseCharacter;
class UTemplateSequence;
class ATemplateSequenceActor;

/** 지속효과 스킬 관련 Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDurationSkillDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndDurationSkillDelegate);

/**
 * 스킬의 종류를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRSkillType : uint8
{
	SkillType_None							UMETA(DisplayName = "None"),
	SkillType_CooldownActiveSkill			UMETA(DisplayName = "CooldownActiveSkill"),
	SkillType_NoneCooldownActiveSkill		UMETA(DisplayName = "NoneCooldonwActiveSkill"),
	SkillType_CooldownPassiveSkill			UMETA(DisplayName = "CooldonwPassiveSkill"),
	SkillType_NoneCooldownPassiveSkill		UMETA(DisplayName = "NoneCooldownPassiveSkill"),
	SkillType_ActiveBuffSkill				UMETA(DisplayName = "ActiveBuffSkill")
};

/**
 * 스킬을 실행할 수 있는 유형(지상에서만 실행 or 공중에서만 실행 or 지상공중 모두 실행)을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRSkillActivatableType : uint8
{
	SkillActivatableType_All		UMETA(DisplayName = "All"),
	SkillActivatableType_Ground		UMETA(DisplayName = "Ground"),
	SkillActivatableType_InAir		UMETA(DisplayName = "InAir")
};

/**
 * 스킬을 실행하는 커맨드를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCommandSkill : uint8
{
	CommandSkill_None					UMETA(DisplayName = "None"),
	CommandSkill_NormalAttack			UMETA(DisplayName = "NormalAttack"),
	CommandSkill_ExtraAttack			UMETA(DisplayName = "ExtraAttack"),
	CommandSkill_FirstBattleSkill		UMETA(DisplayName = "FirstBattleSkill"),
	CommandSkill_SecondBattleSkill		UMETA(DisplayName = "SecondBattleSkill"),
	CommandSkill_ThirdBattleSkill		UMETA(DisplayName = "ThirdBattleSkill"),
	CommandSkill_Ultimate				UMETA(DisplayName = "Ultimate"),
	CommandSkill_GeneralUltimate		UMETA(DisplayName = "GeneralUltimate"),
	CommandSkill_AwakeningUltimate		UMETA(DisplayName = "AwakeningUltimate"),
	CommandSkill_CoreSkill				UMETA(DosplayName = "CoreSkill"),
	CommandSkill_Guard					UMETA(DisplayName = "Guard"),
	CommandSkill_Parry					UMETA(DisplayName = "Parry"),
	CommandSkill_DodgeAttack			UMETA(DisplayName = "DodgeAttack"),
	CommandSkill_ExtremeDodge			UMETA(DisplayName = "ExtremeDodge")
};

/**
 * 스킬에 대한 정보를 변수로 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRSkillInfo()
		: Name()
		, Description()
		, SkillType()
		, Cooldown()
		, Duration()
		, Damage()
		, MaxActivatableCount(-1)
		, SkillIcon()
		, CommandSkill()
		, ActivatableType()
	{}

	FPRSkillInfo(FText NewName, FText NewDescription, EPRSkillType NewSkillType, float NewCooldown, float NewDuration,
					float NewDamage, int32 NewMaxActivatableCount, UTexture2D* NewSkillIcon, EPRCommandSkill NewCommandSkill,
					EPRSkillActivatableType NewActivatableType)
		: Name(NewName)
		, Description(NewDescription)
		, SkillType(NewSkillType)
		, Cooldown(NewCooldown)
		, Duration(NewDuration)
		, Damage(NewDamage)
		, MaxActivatableCount(NewMaxActivatableCount)
		, SkillIcon(NewSkillIcon)
		, CommandSkill(NewCommandSkill)
		, ActivatableType(NewActivatableType)
	{}

public:
	/** 스킬의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	FText Name;

	/** 스킬에 대한 설명입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	FText Description;
	
	/** 스킬의 종류입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	EPRSkillType SkillType;

	/** 스킬의 재사용 대기시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	float Cooldown;

	/** 스킬의 지속시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	float Duration;	

	/** 스킬의 대미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	float Damage;

	/** 스킬의 실행가능 최대 횟수입니다. 스택형태로 축적합니다. 이 기능을 사용하지 않을 경우 값은 -1을 사용합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	int32 MaxActivatableCount;

	/** 스킬의 아이콘입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	UTexture2D* SkillIcon;

	/** 스킬을 실행하는 커맨드를 나타내는 열거형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	EPRCommandSkill CommandSkill;

	/** 스킬을 실행할 수 있는 유형(지상에서만 실행 or 공중에서만 실행 or 지상공중 모두 실행)을 나타내는 열거형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	EPRSkillActivatableType ActivatableType;

	// /** 스킬에서 사용하는 오브젝트의 정보를 가진 구조체들입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillInfo")
	// TArray<FPRPooledObjectInfo> ObjectInfos;
	//
	// /** 스킬에서 사용하는 Niagara Effect의 정보를 가진 구조체들입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillInfo")
	// TArray<FPRNiagaraEffectInfo> NiagaraEffectInfos;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillInfo")
	// TArray<FPRParticleEffectInfo> ParticleEffectInfos;
	
public:
	bool IsEqualActivatableType(EPRSkillActivatableType NewActivatableType) const
	{
		return ActivatableType == NewActivatableType;
	}

public:
	FPRSkillInfo& operator=(const FPRSkillInfo& NewSkillInfo)
	{
		this->Name = NewSkillInfo.Name;
		this->Description = NewSkillInfo.Description;
		this->SkillType = NewSkillInfo.SkillType;
		this->Cooldown = NewSkillInfo.Cooldown;
		this->Duration = NewSkillInfo.Duration;
		this->Damage = NewSkillInfo.Damage;
		this->MaxActivatableCount = NewSkillInfo.MaxActivatableCount;
		this->SkillIcon = NewSkillInfo.SkillIcon;
		this->CommandSkill = NewSkillInfo.CommandSkill;
		this->ActivatableType = NewSkillInfo.ActivatableType;
		// this->ObjectInfos = NewSkillInfo.ObjectInfos;
		// this->NiagaraEffectInfos = NewSkillInfo.NiagaraEffectInfos;
		// this->ParticleEffectInfos = NewSkillInfo.ParticleEffectInfos;

		return *this;
	}
};

/**
 * 캐릭터가 사용하는 스킬의 베이스 클래스입니다.
 */
UCLASS(BlueprintType, Blueprintable, config=Engine)
class PROJECTREPLICA_API UPRBaseSkill : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
	
public:	
	UPRBaseSkill();

#pragma region TickableGameObject
public:
	virtual void Tick(float DeltaTime) override;
	/** Tick을 활성화할지 여부를 반환하는 함수입니다. */
	virtual bool IsTickable() const override;
	/** 에디터에서 Tick을 활성화할지 여부를 반환하는 함수입니다. 일반적으로 true를 반환하여 에디터에서도 객체의 Tick을 활성화할 수 있도록 설정합니다. */
	virtual bool IsTickableInEditor() const override;
	/** 게임이 일시 정지되었을 때 Tick을 계속해서 호출할지 여부를 반환하는 함수입니다. */
	virtual bool IsTickableWhenPaused() const override;
	/** 객체의 고유한 통계 식별자를 반환하는 함수입니다. */
	virtual TStatId GetStatId() const override;
	/** 객체가 존재하는 World를 반환하는 함수입니다. */
	virtual UWorld* GetWorld() const override;

protected:
	/** Tick 함수의 활성화/비활성화를 설정하는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TickableGameObject")
	bool bTickable;

	/** 게임이 일시 정지되었을 때 Tick 함수를 호출할지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TickableGameObject")
	bool bTickableWhenPaused;
#pragma endregion 

public:
	/** 스킬을 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	void InitializeSkill();
	virtual void InitializeSkill_Implementation();
	
	/**
	 * 스킬을 실행하는 함수입니다.
	 *
	 * @return 스킬이 정상적으로 실행됐을 경우 true를 반환합니다. 실행되지 않았을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool ActivateSkill();
	virtual bool ActivateSkill_Implementation();

	/** 스킬이 실행 중인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, category = "BaseSkill")
	bool IsActivateSkill() const;
	
	/** 스킬을 실행할 수 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool IsCanActivateSkill() const;
	virtual bool IsCanActivateSkill_Implementation() const;

	/** 스킬을 실행할 수 있는 유형인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "BaseSkill")
	bool IsCanActivatableType() const;

	/**
	 * AN_PROnSkill 클래스의 Notify 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간에서 실행하고 싶을 때 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool OnSkillNotify();
	virtual bool OnSkillNotify_Implementation();

	/**
	 * ANS_PROnSkill 클래스의 NotifyBegin 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간이 시작될 때 실행하고 싶을 때 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool OnSkillNotifyBegin();
	virtual bool OnSkillNotifyBegin_Implementation();
	
	/**
	 * ANS_PROnSkill 클래스의 NotifyTick 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간을 지나는 동안 실행하고 싶을 때 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool OnSkillNotifyTick(float DeltaTime);
	virtual bool OnSkillNotifyTick_Implementation(float DeltaTime);

	/**
	 * ANS_PROnSkill 클래스의 NotifyEnd 함수에서 실행되는 함수입니다.
	 * 애니메이션이 재생될 때 특정 구간이 끝날 때 실행하고 싶을 때 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSkill")
	bool OnSkillNotifyEnd();
	virtual bool OnSkillNotifyEnd_Implementation();

protected:
	/** 스킬의 실행 상태를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseSkill")
	bool bActivateSkill;

public:
	/** 입력받은 인자로 bActivateSkill을 설정하는 함수입니다. */
	virtual void SetActivateSkill(bool bNewActivateSkill);

#pragma region SkillInfo
protected:
	/** 스킬의 정보입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillInfo")
	FPRSkillInfo SkillInfo;

	/** 스킬을 실행할 수 있는 횟수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkillInfo")
	int32 ActivateableCount;

private:
	/** 스킬 사용자입니다 .*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillInfo", meta = (AllowPrivateAccess = "true"))
	APRBaseCharacter* SkillOwner;

public:
	/** SkillInfo를 반환하는 함수입니다. */
	FPRSkillInfo GetSkillInfo() const { return SkillInfo; }

	/** 입력받은 인자로 SkillInfo를 설정하는 함수입니다. */
	void SetSkillInfo(FPRSkillInfo NewSkillInfo);

	/** SkillOwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetSkillOwner() const { return SkillOwner; }

	/** 입력받은 인자로 SkillOwner를 설정하는 함수입니다. */
	void SetSkillOwner(class APRBaseCharacter* NewSkillOwner);

	/** ActivateableCount를 반환하는 함수입니다. */
	float GetActivateableCount() const { return ActivateableCount; }

	/** 입력받은 인자로 ActivateableCount를 설정하는 함수입니다. */
	void SetActivateableCount(int32 NewActivateableCount);
#pragma endregion

#pragma region Cooldown
public:
	/** 재사용 대기시간을 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Cooldown")
	void ActivateCooldown();
	virtual void ActivateCooldown_Implementation();

	/** 재사용 대기시간이 실행되고 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	virtual bool IsCooldown() const;

	/** 재사용 대기시간의 남은 시간을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	virtual float GetSkillCooldownRemaining() const;

	/** 재사용 대기시간의 남은 시간의 비율을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	virtual float GetSkillCooldownRemainingRatio() const;

	/** 재사용 대기시간의 경과 시간을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	virtual float GetSkillCooldownElapsed() const;

	/** 재사용 대기시간의 경과 시간의 비율을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	virtual float GetSkillCooldownElapsedRatio() const;
#pragma endregion

#pragma region DurationEffect
public:
	/** 스킬의 지속효과를 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DurationEffect")
	virtual void ActivateDuration();

	/** 스킬의 지속효과의 남은 시간을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DurationEffect")
	float GetRemainingDurationEffect() const;

protected:
	/** 지속효과를 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DurationEffect")
	virtual void DurationEffect(); 
	
	/** 지속효과가 종료되었을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DurationEffect")
	virtual void EndDurationEffect();

	/** 지속효과를 최신화하는 함수입니다. */
	// UFUNCTION(BlueprintCallable, Category = "DurationEffect")
	// virtual void UpdateDurationEffect(float DeltaTime);

protected:
	/** 지속효과에 사용하는 TimerHandle입니다. */
	FTimerHandle DurationTimerHandle;

	/** 지속효과의 실행을 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DurationEffect")
	bool bActivateDurationEffect;

	/** 지속효과의 남은 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DurationEffect")
	float DurationEffectRemaining;

	/** 지속효과의 경과 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DurationEffect")
	float DurationEffectElapsed;
	
public:
	/** 지속스킬이 실행됐을 때 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DurationEffect")
	FOnDurationSkillDelegate DurationSkillDelegate;

	/** 지속스킬이 종료되었을 때 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DurationEffect")
	FOnEndDurationSkillDelegate EndDurationSkillDelegate;
#pragma endregion

#pragma region SkillCutScene
public:
	/** 스킬 컷신을 재생하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SkillCutScene")
	void PlaySkillCutScene();
	void PlaySkillCutScene_Implementation();

	/** 스킬 컷신의 재생이 끝난 후 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SkillCutScene")
	void OnFinishedSkillCutScene();
	void OnFinishedSkillCutScene_Implementation();

protected:
	/** 스킬의 컷신입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillCutScene")
	UTemplateSequence* SkillCutScene;

	/** 스킬의 컷신의 재생옵션입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillCutScene")
	FMovieSceneSequencePlaybackSettings SkillCutScenePlaybackSettings;

	/** 스킬 컷신의 시퀸스 액터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkillCutScene")
	ATemplateSequenceActor* SkillCutSceneSequenceActor;
#pragma endregion
};
