// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRPlayerCharacter.h"
#include "PRPlayerCharacter_Kyle.generated.h"

/**
 * 캐릭터의 공격 모드를 나타내는 열거형입니다.
 * General: 일반 모드
 * Awakening: 각성 모드
 */
UENUM(BlueprintType)
enum class EPRAttackMode : uint8
{
	AttackMode_General			UMETA(DisplayName = "General"),
	AttackMode_Awakening		UMETA(DisplayName = "Awakening")
};

/**
 * 카일 플레이어 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerCharacter_Kyle : public APRPlayerCharacter
{
	GENERATED_BODY()

public:
	APRPlayerCharacter_Kyle();
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma region AnimSystem
private:
	/** 캐릭터가 사용하는 PRAniMontage들을 초기화하는 함수입니다. */
	virtual void InitializePRAnimMontages() override;
#pragma endregion

#pragma region MovementInput
protected:
	virtual void Jump() override;
	virtual void StopJumping() override;
	
	/**
	 * 방향에 따라 AddMovementInput을 호출하여 이동하는 함수입니다.
	 * 
	 * @param Direction 이동하는 방향입니다.
	 */
	virtual void AddPlayerMovementInput(EPRDirection Direction) override;
#pragma endregion 

#pragma region CharacterInfo
private:
	/**
	 * 캐릭터의 공격 모드를 나타내는 열거형입니다.
	 * AttackMode가 General일 경우 한손검을 사용하여 공격합니다.
	 * AttackMode가 Awakening일 경우 이도류를 사용하여 공격합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfo", meta = (AllowPrivateAccess = "true"))
	EPRAttackMode AttackMode;
#pragma endregion

#pragma region Dodge
protected:
	/** 회피를 실행하는 함수입니다. */
	virtual void Dodge() override;
#pragma endregion 

#pragma region CommandInput
protected:
	virtual void NormalAttack() override;
	
	// virtual void ChargedAttack(FKey Key) override;
#pragma endregion

#pragma region NormalAttack
public:
	/** PlayNormalAttackIndex를 초기화하는 함수입니다. */
	virtual void InitializePlayNormalAttackIndex() override;
	
	/**
	 * PlayNormalAttackIndex를 1 증가 시키는 함수입니다.
	 * PlayNormalAttackIndex가 현재 재생하는 AttackPRAnimMontage를 보관한 Array의 크기와 같을 경우 0으로 설정합니다.
	 */
	virtual void IncreasePlayNormalAttackIndex() override;
	
private:
	/** 일반 공격을 실행하는 함수입니다. */
	void ExecuteNormalAttack();
	
private:
	/** General 모드의 NormalAttack에 사용하는 PRAnimMontage의 시작 ID입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 GeneralNormalAttackPRAnimMontageFromID;

	/** General 모드의 NormalAttack에 사용하는 PRAnimMontage의 끝 ID입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 GeneralNormalAttackPRAnimMontageToID;
	
	/** Awakening 모드의 NormalAttack에 사용하는 PRAnimMontage의 시작 ID입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 AwakeningNormalAttackPRAnimMontageFromID;

	/** Awakening 모드의 NormalAttack에 사용하는 PRAnimMontage의 끝 ID입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (ClampMin = "1000", ClampMax = "1999", AllowPrivateAccess = "true"))
	int32 AwakeningNormalAttackPRAnimMontageToID;

	/** General 모드에서 NormalAttack에 사용하는 PRComboAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
	FPRComboAnimMontage GeneralNormalPRComboAnimMontage;

	/** Awakening 모드에서 NormalAttack에 사용하는 PRComboAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
	FPRComboAnimMontage AwakeningNormalPRComboAnimMontage;
#pragma endregion

#pragma region ChargedAttack
private:
// private:
// 	/** ChargedAttack을 실행하는 함수입니다. */
// 	UFUNCTION()
// 	void ExecuteChargedAttack();
// 	
// private:
// 	/** ChargedAttack의 PRAnimMontage들을 보관한 데이터 테이블입니다. */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
// 	UDataTable* ChargedAttackPRAnimMontageDataTable;
//
// 	/** ChargedAttack에 사용하는 PRAnimMontage의 시작 ID입니다. */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
// 	int32 ChargedNormalAttackPRAnimMontageFromID;
//
// 	/** ChargedAttack에 사용하는 PRAnimMontage의 끝 ID입니다. */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
// 	int32 ChargedNormalAttackPRAnimMontageToID;
//
// 	/** ChargedAttack에 사용하는 PRAnimMontage들을 보관한 Array입니다. */
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NormalAttack", meta = (AllowPrivateAccess = "true"))
// 	TArray<FPRAnimMontage> ChargedAttackPRAnimMontages;
// 	
// 	/** ChargedAttack의 실행을 나타내는 변수입니다. */
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ChargedAttack", meta = (AllowPrivateAccess = "true"))
// 	bool bExecuteChargedAttack;
// 	
// 	/** ChargedAttack을 하기 위한 입력키의 홀드 시간입니다. */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChargedAttack", meta = (AllowPrivateAccess = "true"))
// 	float ChargedAttackableHoldTime;
//
// 	/** ChargedAttack 입력 키입니다.*/
// 	UPROPERTY(BlueprintReadWrite, Category = "ChargedAttack", meta = (AllowPrivateAccess = "true"))
// 	FKey ChargedAttackInputKey;
//
// 	/** ChargedAttack에 사용하는 PRAnimMontage입니다. */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargedAttack", meta = (AllowPrivateAccess = "true"))
// 	FPRAnimMontage ChargedAttackPRAnimMontage;
#pragma endregion

#pragma region DodgeAttack
protected:
	/** 입력받은 인자로 DodgeAttack의 활성화/비활성화를 설정하는 함수입니다. */
	void SetActivateDodgeAttack(bool bActivate);
#pragma endregion 
};
