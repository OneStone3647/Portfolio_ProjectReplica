// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "PRAICharacter.generated.h"

class APRAISpawner;
class UPRTargetAimSystemComponent;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIDeactivate, APRAICharacter*, AICharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDynamicAIDeactivate, APRAICharacter*, AICharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);


/**
 * AI 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRAICharacter : public APRBaseCharacter
{
	GENERATED_BODY()

public:
	APRAICharacter();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void TakeHit(AActor* DamageCauser) override;
	virtual void Dead() override;

#pragma region Activate
public:
	/** AI 캐릭터가 활성화되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Activate")
	bool IsActivate() const;	
	
	/** AI 캐릭터를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Activate")
	void Activate();

	/** AI 캐릭터를 비활성화 하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Activate")
	void Deactivate();

protected:
	/** AI 캐릭터의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activate")
	bool bActivate;
	
	/** AI 캐릭터의 수명입니다. 수명이 다할 경우 AI 캐릭터는 비활성화됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate", meta = (ClampMin = "0"))
	float Lifespan;

public:
	/** AI가 비활성화될 때 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Activate")
	FOnAIDeactivate OnAIDeactivate;

	/** 동적으로 생성한 AI가 비활성화될 때 OnAIDeactivate와 함께 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "Activate")
	FOnDynamicAIDeactivate OnDynamicAIDeactivate;
#pragma endregion

#pragma region TimeStop
public:
	/** TimeStop에 영향을 받았는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	bool IsTimeStopActive() const;

	/** TimeStop에 영향을 받을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void TimeStopActive();

	/** TimeStop에 영향을 받지 않을 때 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void TimeStopDeactive();
	
protected:
	/** TimeStop에 영향을 받았는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeStop")
	bool bTimeStopActive;
#pragma endregion 

#pragma region TargetAimSystem
public:
	/**
	 * Target으로 지정되었을 때 실행하는 함수입니다.
	 *
	 * @param bActivateLockedOn TargetAimSystem의 LockOnWidget의 활성화를 설정합니다.
	 */
	void LockedOnTarget(bool bActivateLockedOn);
	
protected:
	/** Targeting TraceChannel에 블록되는 Collision과 Target으로 지정되어 LockOn 되었을 때 활성화하는 위젯을 가진 SphereComponent 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetAimSystem")
	UPRTargetAimSystemComponent* TargetAimSystem;

public:
	/** TargetAimSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTargetAimSystemComponent* GetTargetAimSystem() const
	{
		return TargetAimSystem;
	}
	
public:
	/** TargetAim의 위치를 반환하는 함수입니다. */
	FVector GetTargetAimLocation() const;
#pragma endregion

#pragma region HealthPointBar
protected:
	/** HealthPointBar를 초기화하는 함수입니다. */
	void InitializeHealthPointBar();

	/** HealthPointBar가 PlayerCamera를 바라보도록 설정하는 함수입니다. */
	void HealthPointBarLookAtPlayerCamera();

private:
	/** 캐릭터의 HealthPoint를 나타내는 WidgetComponent입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HealthPointBar;

	/** 일정시간 피격 당하지 않으면 HealthPointBar를 숨기는 TimerHandle입니다. */
	FTimerHandle HealthPointBarHiddenTimerHandle;

	/** 피격당한 이후 다시 피격당하지 않아 HealthPointBar를 숨길 때까지 걸리는 시간입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthPointBar", meta = (AllowPrivateAccess = "true"))
	float HealthPointBarVisibleTimeSinceTakeHit;

public:
	/** HealthPointBart을 반환하는 함수입니다. */
	class UWidgetComponent* GetHealthPointBar() const;
#pragma endregion

#pragma region Spawner
protected:
	/** AI 캐릭터를 Spawn한 Spawner입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spawner")
	APRAISpawner* Spawner;

public:
	/** Spawner를 반환하는 함수입니다. */
	APRAISpawner* GetSpawner() const;
	
	/** 입력받은 인자로 Spawner를 설정하는 함수입니다. */
	void SetSpawner(APRAISpawner* NewSpawner);
#pragma endregion

#pragma region PoolIndex
protected:
	/** AIPool의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIListIndex")
	int32 PoolIndex;

public:
	/** PoolIndex를 반환하는 함수입니다. */
	int32 GetPoolIndex() const;

	/** 입력받은 인자로 PoolIndex를 설정하는 함수입니다. */
	void SetPoolIndex(int32 NewPoolIndex);
#pragma endregion

#pragma region AI
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void DefaultAttack();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackAnimMontage;
	
public:
	/** 공격을 끝냈을 때 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "AI")
	FOnAttackEnd OnAttackEnd;
#pragma endregion 
};
