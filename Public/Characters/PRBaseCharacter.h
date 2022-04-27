// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Enums/Enum_PRGait.h"
#include "Enums/Enum_PRRotationMode.h"
#include "GameFramework/Character.h"
#include "PRBaseCharacter.generated.h"

/**
 * 캐릭터의 바탕이 되는 Character 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APRBaseCharacter();

protected:
	/** 액터에 속한 모든 컴포넌트의 세팅이 완료되면 호출되는 함수입니다. */
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	/** 캐릭터가 사망했는지 판별하는 함수입니다. */
	bool IsDead() const;
	
protected:
	/**
	 * 피격 당했을 때 호출하는 함수입니다.
	 * 공격자를 향해 바라보고 피격 애니메이션을 재생합니다.
	 * @param DamageCauser 공격자입니다.
	 */
	virtual void TakeHit(AActor* DamageCauser);

	/** 캐릭터가 사망할 때 호출하는 함수입니다. */
	UFUNCTION()
	virtual void Dead();
	
#pragma region MovementSystem
protected:
	/** 캐릭터의 움직임을 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|MovementSystem")
	class UPRMovementSystemComponent* MovementSystem;

public:
	/** MovementSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRMovementSystemComponent* GetMovementSystem() const
	{
		return MovementSystem;
	}

	/** MovementSystem의 SetRotationMode 함수를 실행하는 함수입니다. */
	void SetRotationMode(EPRRotationMode NewRotationMode);
#pragma endregion

#pragma region StateSystem
protected:
	/** 캐릭터의 상태를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRCharacter|StateSystem")
	class UPRStateSystemComponent* StateSystem;

public:
	/** StateSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStateSystemComponent* GetStateSystem() const
	{
		return StateSystem;
	}
#pragma endregion

#pragma region StatSystem
protected:
	/* 캐릭터의 스탯(능력치)를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRCharacter|StatSystem")
	class UPRStatSystemComponent* StatSystem;

public:
	/** StatSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStatSystemComponent* GetStatSystem() const
	{
		return StatSystem;
	}
#pragma endregion 

#pragma region AnimSystem
protected:
	/** 캐릭터의 애니메이션을 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|AnimSystem")
	class UPRAnimSystemComponent* AnimSystem;

public:
	/** AnimSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRAnimSystemComponent* GetAnimSystem() const
	{
		return AnimSystem;
	}
#pragma endregion

#pragma region WeaponSystem
protected:
	/** 캐릭터의 무기를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|WeaponSystem")
	class UPRWeaponSystemComponent* WeaponSystem;

public:
	/** WeaponSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRWeaponSystemComponent* GetWeaponSystem() const
	{
		return WeaponSystem;
	}
#pragma endregion 
	
#pragma region DodgeSystem
protected:
	/** 캐릭터의 회피를 실행하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|DodgeSystem")
	class UPRDodgeSystemComponent* DodgeSystem;

public:
	/** DodgeSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRDodgeSystemComponent* GetDodgeSystem() const
	{
		return DodgeSystem;
	}
#pragma endregion

#pragma region CharacterMovement
protected:
	/**
	 * 입력받은 인자에 따라 캐릭터가 달리는지 전력질주하는지 설정합니다.
	 * @NewPRGait: 설정할 캐릭터의 걷는 모양입니다.
	 */
	void SetRunOrSprint(EPRGait NewPRGait);

	/**
	 * 착지하고 바로 달렸을 때 발소리와 착지 소리를 출력하는 함수입니다.
	 * AnimNotify로 구현하지 못하였기에 이렇게 구현합니다.
	 */
	void LandedAndAccelerating();
#pragma endregion

#pragma region AudioComponent
protected:
	/** 캐릭터의 발소리 AudioComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|Audio")
	UAudioComponent* FootStepAudio;

	/** 발소리 출력 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRJump", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FootStepVolume;
	
	/** 캐릭터의 점프소리 AudioComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRCharacter|Audio")
	UAudioComponent* JumpAudio;
	
	/** 점프 목소리 출력 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRJump", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float JumpVoiceVolume;

public:
	FORCEINLINE UAudioComponent* GetFootStepAudio() const
	{
		return FootStepAudio;
	}

	/** FootStepVolume을 반환하는 함수입니다. */
	float GetFootStepVolume() const;

	FORCEINLINE UAudioComponent* GetJumpAudio() const
	{
		return JumpAudio;
	}

	/** JumpVoiceVolume을 반환하는 함수입니다. */
	float GetJumpVoiceVolume() const;
#pragma endregion

#pragma region Dodge
protected:
	/** 캐릭터가 회피하는 함수입니다. */
	virtual void Dodge();
#pragma endregion 

#pragma region Attack
	/** 캐릭터가 공격하는 함수입니다. */
	virtual void Attack();
#pragma endregion 
};
