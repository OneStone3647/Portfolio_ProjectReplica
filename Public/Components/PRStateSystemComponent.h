// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRStateSystemComponent.generated.h"

/**
 * 캐릭터의 상태를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRStateSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStateSystemComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Debug
public:
	/** 디버그를 최신화하는 함수입니다. */
	void UpdateDebug();

protected:
	/** 디버그 실행을 결정하는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterState|Debug")
	bool bActivateDebug;
#pragma endregion

#pragma region TakeDamage
public:
	/** 캐릭터가 피격 당했는지 판별하는 함수입니다. */
	bool IsHit() const;

	/** 캐릭터가 사망했는지 판별하는 함수입니다. */
	bool IsDead() const;

	/** 캐릭터가 무적 상태인지 판별하는 함수입니다. */
	bool IsInvincible() const;

private:
	/** 캐릭터가 피격 당했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState|TakeDamage", Meta = (AllowPrivateAccess = "true"))
	bool bIsHit;

	/** 캐릭터가 사망했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState|TakeDamage", Meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	/**
	 * 캐릭터가 무적 상태인지 나타내는 변수입니다.
	 * 무적 상태의 캐릭터는 대미지를 받지않으며 피격 애니메이션을 재생하지 않습니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState|TakeDamage", Meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible;

public:
	/** 입력받은 인자로 bIsHit을 설정하는 함수입니다. */
	void SetIsHit(bool bNewIsHit);

	/** 입력받은 인자로 bIsDead를 설정하는 함수입니다. */
	void SetIsDead(bool bNewIsDead);

	/** 입력받은 인자로 bIsInvincible를 설정하는 함수입니다. */
	void SetIsInvincible(bool bNewIsInvincible);
#pragma endregion 
	
public:
	/** 캐릭터가 액션을 캔슬할 수 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CharacterState")
	bool IsCanCancelAction() const;
	
	/** ActionState를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CharacterState")
	void InitializeActionState();

	/** 입력받은 인자의 액션을 할 수 있는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CharaterState")
	bool IsActionable(EPRAction Action) const;

	/**
	 * 입력받은 인자에 해당하는 액션의 값을 설정하는 함수입니다.
	 *
	 * @param Action 값을 변경할 액션입니다.
	 * @param bAble 액션 가능 값을 나타내는 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "CharaterState")
	void SetActionable(EPRAction Action, bool bAble);

	/**
	 * 입력받은 인자에 해당하는 액션들의 값을 설정하는 함수입니다.
	 *
	 * @param Actions 값을 변경할 액션과 액션 가능 값을 나타내는 Map입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "CharaterState")
	void SetActionables(TMap<EPRAction, bool>& Actions);
	
	/**
	 * NewDelayTime만큼 시간이 지난 후 입력받은 인자로 bDodgeable을 설정하는 함수입니다.
	 * @param bNewDodgeable bDodgeable을 설정할 매개변수입니다.
	 * @param NewDelayTime bDodgeable을 설정할 때 적용되는 딜레이 시간입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "CharacterState")
	void SetDodgealbeAfterDelay(bool bNewDodgeable, float NewDelayTime);
	
private:
	/** 캐릭터가 액션을 캔슬할 수 있는 상태를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	bool bCanCancelAction;
	
	/**
	 * 캐릭터의 액션 가능 상태를 나타내는 Map입니다.
	 *
	 * @Key 액션의 종류입니다.
	 * @Value 액션 가능 상태입니다. 액션이 실행 가능한 상태일 경우 true, 실행 불가능한 상태일 경우 false를 나타냅니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	TMap<EPRAction, bool> ActionState;

	/** 딜레이를 주고 bDodgeable을 설정할 때 사용하는 TimerHandle입니다. */
	FTimerHandle DodgeableDelayTimerHandle;
	
public:
	/** 입력받은 인자로 bCanCancelAction을 설정하는 함수입니다. */
	void SetCanCancelAction(bool bNewCanCancelAction);
};
