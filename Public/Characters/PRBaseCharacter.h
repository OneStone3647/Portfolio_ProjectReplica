// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Common/PRCommonStruct.h"
#include "GameFramework/Character.h"
#include "PRBaseCharacter.generated.h"

class UPRAnimSystemComponent;
class UPRMovementSystemComponent;
class UPRStateSystemComponent;
class UPRStatSystemComponent;
class UPRWeaponSystemComponent;
class UPRSkillSystemComponent;
class UPRObjectPoolSystemComponent;
class UPREffectSystemComponent;

struct FPRWeaponGroup;

/**
 * 기본 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APRBaseCharacter();

protected:
	virtual void PostInitializeComponents() override;	// 액터에 속한 모든 컴포넌트의 세팅이 완료되면 호출되는 함수입니다. 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

#pragma region TakeDamage
public:
	/** 캐릭터가 사망했는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseCharacter|TakeDamage")
	bool IsDead() const;
	
protected:
	/**
	 * 캐릭터가 피격 당했을 때 호출하는 함수입니다.
	 * 공격자를 향해 바라보고 피격 애니메이션을 재생합니다.
	 *
	 * @param DamageCauser 캐릭러를 공격한 공격자입니다.
	 */
	virtual void TakeHit(AActor* DamageCauser);

	/** 캐릭터가 사망했을 때 호출하는 함수입니다. */
	UFUNCTION()
	virtual void Dead();

protected:
	/** 피격 애니메이션입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseCharater|TakeDamage")
	FPRAnimMontage HitAnimMontage;
#pragma endregion 

#pragma region MovementInput
public:
	/** 캐릭터가 점프했는지 판별하는 함수입니다. */
	bool IsJumped() const;
	
protected:
	virtual void Jump() override;

protected:
	/** 캐릭터가 점프했는지 나타내는 변수입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovementInput")
	bool bJumped;
	
	/** Jumped를 false로 설정할 때 딜레이를 주는 TimerHandle입니다. */
	FTimerHandle JumpedDelayTimerHandle;
#pragma endregion

#pragma region AnimSystem
protected:
	/** 캐릭터가 사용하는 PRAnimMontage들을 초기화하는 함수입니다. */
	virtual void InitializePRAnimMontages();
	
private:
	/** 캐릭터가 재생하는 PRAnimMontage를 관리하고 재생하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimSystem", meta = (AllowPrivateAccess = "true"))
	UPRAnimSystemComponent* AnimSystem;

public:
	/** AnimSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRAnimSystemComponent* GetAnimSystem() const { return AnimSystem; }
#pragma endregion

#pragma region MovementSystem
private:
	/** 캐릭터의 움직임을 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementSystem", meta = (AllowPrivateAccess = "true"))
	UPRMovementSystemComponent* MovementSystem;

public:
	/** MovementSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRMovementSystemComponent* GetMovementSystem() const	{ return MovementSystem; }
#pragma endregion

#pragma region StateSystem
private:
	/** 캐릭터의 상태를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateSystem", meta = (AllowPrivateAccess = "true"))
	UPRStateSystemComponent* StateSystem;

public:
	/** StateSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStateSystemComponent* GetStateSystem() const { return StateSystem;	}
#pragma endregion 

#pragma region StatSystem
private:
	/** 캐릭터의 스탯을 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatSystem", meta = (AllowPrivateAccess = "true"))
	UPRStatSystemComponent* StatSystem;

public:
	/** StatSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStatSystemComponent* GetStatSystem() const { return StatSystem; }
#pragma endregion

#pragma region WeaponSystem
private:
	/** 캐릭터의 무기를 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponSystem", meta = (AllowPrivateAccess = "true"))
	UPRWeaponSystemComponent* WeaponSystem;

public:
	/** WeaponSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRWeaponSystemComponent* GetWeaponSystem() const	{ return WeaponSystem; }

	// /** 현재 장비한 무기를 반환하는 함수입니다. */
	// FPRWeaponGroup GetEquippedWeaponGroup() const;
#pragma endregion

#pragma region SkillSystem
private:
	/** 캐릭터의 스킬을 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillSystem", meta = (AllowPrivateAccess = "true"))
	UPRSkillSystemComponent* SkillSystem;

public:
	/** SkillSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRSkillSystemComponent* GetSkillSystem() const { return SkillSystem; }
#pragma endregion

#pragma region ObjectPoolSystem
protected:
	/** 오브젝트 풀을 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	UPRObjectPoolSystemComponent* ObjectPoolSystem;

public:
	/** ObjectPoolSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRObjectPoolSystemComponent* GetObjectPoolSystem() const { return ObjectPoolSystem; }
#pragma endregion

#pragma region EffectSystem
private:
	/** 플레이어가 사용하는 Effect를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EffectSystem", meta = (AllowPrivateAccess = "true"))
	UPREffectSystemComponent* EffectSystem;

public:
	/** EffectSystem을 반환하는 함수입니다. */
	UPREffectSystemComponent* GetEffectSystem() const {	return EffectSystem; }
#pragma endregion

#pragma region Dodge
protected:
	/** 회피에 사용하는 PRAnimMontage들을 초기화하는 함수입니다. */
	void InitializeDodgePRAnimMontages();
	
	/** 회피를 실행하는 함수입니다. */
	virtual void Dodge();

protected:
	/** 회피 PRAnimMontage를 보관한 Map입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dodge")
	TMap<FName, FPRAnimMontage> DodgePRAnimMontages;
	
	/** 전방 회피 PRAnimMontage의 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	int32 ForwardDodgePRAnimMontageID;

	/** 후방 회피 PRAnimMontage의 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	int32 BackwardDodgePRAnimMontageID;

	/** 좌방 회피 PRAnimMontage의 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	int32 LeftDodgePRAnimMontageID;

	/** 우방 회피 PRAnimMontage의 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	int32 RightDodgePRAnimMontageID;

	/** 공중 전방 회피 PRAnimMontage의 ID 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	int32 AerialForwardDodgePRAnimMontageID;
#pragma endregion

#pragma region NormalAttack
public:
	/** PlayNormalAttackIndex를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "NormalAttack")
	virtual void InitializePlayNormalAttackIndex();
	
	/**
	 * PlayNormalAttackIndex를 1 증가 시키는 함수입니다.
	 * PlayNormalAttackIndex가 현재 재생하는 AttackPRAnimMontage를 보관한 Array의 크기와 같을 경우 0으로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "NormalAttack")
	virtual void IncreasePlayNormalAttackIndex();
#pragma endregion

#pragma region Effect
protected:
	/** Effect의 시그니처 컬러입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	FLinearColor SignatureEffectColor;

public:
	/** SignatureEffectColor를 반환합는 함수입니다. */
	FLinearColor GetSignatureEffectColor() const;
#pragma endregion
};
