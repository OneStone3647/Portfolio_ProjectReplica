// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRDamageSystemComponent.generated.h"

class UPRStatSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlockedDelegate, bool, CanBeParried);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageResponseDelegate, EPRDamageResponse, DamageResponse);

/**
 * 대미지의 처리를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCanBeDamaged : uint8
{
	CanBeDamaged_BlockDamage		UMETA(DisplayName = "BlockDamage"),
	CanBeDamaged_DoDamage			UMETA(DisplayName = "DoDamage"),
	CanBeDamaged_NoDamage			UMETA(DisplayName = "NoDamage")
};

/**
 * 대미지를 관리하는 ActorComponent 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRDamageSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRDamageSystemComponent();

public:
	virtual void OnRegister() override;

public:
	/** 입력받은 StatSystem을 바인딩하는 함수입니다. */
	void BindStatSystem(UPRStatSystemComponent* NewStatSystem);
	
	/**
	 * 회복량만큼 체력을 회복하고 회복한 후의 체력을 반환하는 함수입니다.
	 *
	 * @param Amount 회복량
	 * @return 회복한 후의 체력
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageSystem")
	float Heal(float Amount);

	/**
	 * 대미지 정보에 해당하는 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 받는 대미지의 정보
	 * @return 대미지를 받을 경우 true, 대미지를 받지 않을 경우 false를 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageSystem")
	bool TakeDamage(FPRDamageInfo DamageInfo);

	// /**
	//  * 사망했는지 판별하는 함수입니다.
	//  *
	//  * @return 사망했을 경우 true, 사망하지 않았을 경우 false를 반환
	//  */
	// UFUNCTION(BlueprintCallable, Category = "DamageSystem")
	// bool IsDead() const;

private:
	/** 받는 대미지의 정보에 따라 방어하여 대지미를 받지 않을지, 대미지를 받을지, 대미지를 받지 않을지 판별하는 함수입니다. */
	EPRCanBeDamaged CanBeDamaged(const bool& bShouldDamageInvincible, const bool& bCanBeBlocked);

private:
	/** 캐릭터의 스탯을 관리하는 ActorComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	UPRStatSystemComponent* StatSystem;
	
	// /** 체력입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	// float Health;
	//
	// /** 최대 체력입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	// float MaxHealth;

	// /** 무적 상태인지 나타내는 변수입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	// bool bIsInvincible;

	// /** 죽었는지 나타내는 변수입니다. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	// bool bIsDead;

	/** 동작을 강제로 중단할 수 있는지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	bool bIsInterruptible;

	/** 방어 상태인지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

// public:
// 	/** Health를 반환하는 함수입니다. */
// 	float GetHealth() const;
//
// 	/** MaxHealth를 반환하는 함수입니다. */
// 	float GetMaxHealth() const;

public:
	/** 사망했을 때 호출하는 델리게이트입니다. */
	FOnDeathDelegate OnDeathDelegate;

	/** 방어했을 때 호출하는 델리게이트입니다. */
	FOnBlockedDelegate OnBlockedDelegate;

	/** 대미지에 반응할 때 호출하는 델리게이트입니다. */
	FOnDamageResponseDelegate OnDamageResponseDelegate;
};
