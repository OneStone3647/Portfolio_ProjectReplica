// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Enums/Enum_PRDirection.h"
#include "PRPlayerCharacter.generated.h"

enum class EPRDirectionVector : uint8;

/**
 * 플레이어 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerCharacter : public APRBaseCharacter
{
	GENERATED_BODY()

public:
	APRPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;
	
#pragma region Camera
protected:
	/**
	 * 마우스 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 * @Value: 마우스를 사용한 좌우 회전값입니다.
	 */
	void Turn(float Value);

	/**
	 * 마우스를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 * @Value: 마우스를 사용한 상하 회전값입니다.
	 */
	void LookUp(float Value);

	/**
	 * 게임패드를 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 * @Rate: 게임패드를 사용한 좌우 회전값입니다.
	 */
	void TurnRate(float Rate);
	
	/**
	 * 게임패드를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 * @Rate: 게임패드를 사용한 상하 회전값입니다.
	 */
	void LookUpRate(float Rate);

	/**
	 * CameraZoomOutTimeline을 초기화하는 함수입니다.
	 * Curve가 사용하는 Callback 함수에 사용할 함수를 바인드합니다.
	 * Timeline에 Curve를 추가합니다.
	 */
	void InitializeCameraZoomOutTimeline();

	/**
	 * 카메라를 줌아웃하는 함수입니다.
	 * @param Value: Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void CameraZoomOut(float Value);

	/**
	 * 캐릭터가 전력질주할 때 카메라를 줌아웃하는 함수입니다.
	 * @param bFlag: true일 경우 카메라를 줌아웃합니다. false일 경우 카메라를 초기상태로 되돌립니다.
	 */
	void CameraZoomOutWhenSprint(bool bFlag = true);

protected:
	/** 캐릭터 뒤에 카메라를 배치하는 SpringArm입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	/** 캐릭터 뒤에서 따라오는 카메라입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	/** 카메라가 캐릭터 뒤에서 떨어져서 따라오는 거리입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseSpringArmLength;

	/** 카메라의 좌우 기본 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** 카메라의 상하 기본 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	/** 카메라를 줌아웃할 때 사용하는 Timeline입니다. */
	FTimeline CameraZoomOutTimeline;

	/** 카메라를 줌아웃할 때 사용하는 Curve입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	UCurveFloat* CameraZoomOutCurveFloat;
#pragma endregion

#pragma region CharacterMovement
public:
	/**
	 * 입력 방향으로 캐릭터를 회전시키는 함수입니다.
	 * @param bIsReverse: true일 경우 입력 방향의 반대방향으로 캐릭터를 회전시킵니다.
	 *						false일 경우 입력방향으로 캐릭터를 회전시킵니다.
	 */
	void RotateInputDirection(bool bIsReverse = false);

	/** InputComponent에서 MoveForward의 값을 반환합니다. */
	float GetMoveForward() const;

	/** InputComponent에서 MoveRight의 값을 반환합니다. */
	float GetMoveRight() const;
	
public:
	/**
	 * 게임패드를 사용할 때 아날로그 스틱의 대각선 입력을 보정하는 함수입니다.
	 * @param NewForwardAxis MoveForward 입력입니다.
	 * @param NewRightAxis MoveRight 입력입니다.
	 * @return Get<0> 보정된 ForwardAxis입니다.
	 * @return Get<1> 보정된 RightAxis입니다.
	 */
	TTuple<float, float> FixDiagonalGamepadValues(float NewForwardAxis, float NewRightAxis) const;
	
	/**
	 * 컨트롤러를 기준으로 전진벡터와 우측벡터를 반환하는 함수입니다.
	 * @return Get<0> 컨트롤러를 기준으로 한 전진벡터입니다.
	 * @return Get<1> 컨트롤러를 기준으로 한 우측벡터입니다. 
	 */
	TTuple<FVector, FVector> GetControlForwardRightVector() const;

	/**
	 * 방향 벡터에 따라 AddMovementInput을 호출하여 이동하는 함수입니다.
	 * @param PRDirectionVector 방향 벡터입니다.
	 */
	void AddPlayerMovementInput(EPRDirectionVector PRDirectionVector);

	/** 플레이어의 이동의 입력 방향을 PRDirection으로 반환하는 함수입니다. */
	EPRDirection GetMoveInputDirection() const;

	/** 캐릭터의 걷는 모양을 토글로 걷기 또는 달리기로 전환하는 함수입니다. */
	void ToggleWalkRun();

	/** Jump 키를 누를 경우 호출하는 함수입니다. */
	virtual void Jump() override;

	/** DoubleJump를 실행하는 함수입니다. */
	void DoubleJump();

	/** DoubleJump의 NiagaraEffect를 실행하는 함수입니다. */
	void ExecuteDoubleJumpNiagaraEffect();
	
	/** DoubleJump인지를 반환하는 함수입니다. */
	bool IsDoubleJump() const;

protected:
	/**
	 * MoveForward 키를 입력할 경우 호출하는 함수입니다.
	 * @param Value MoveForward의 입력값입니다.
	 */
	void MoveForward(float Value);

	/**
	 * MoveRight 키를 입력할 경우 호출하는 함수입니다.
	 * @param Value MoveRight의 입력값입니다.
	 */
	void MoveRight(float Value);

private:
	/** DoubleJump 애니메이션 몽타주입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DoubleJumpAnimMontage;
	
	/** DoubleJump NiagaraEffect입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* DoubleJumpNiagaraEffect;

	/** DoubleJump NiagaraEffect의 색입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	FLinearColor DoubleJumpNiagaraEffectColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	FName DoubleJumpNiagaraEffectSpawnSocketName;
	
	/** DoubleJump인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	bool bIsDoubleJump;

	/** DoubleJump를 할 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	bool bCanDoubleJump;

	/** DoubleJump의 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement|DoubleJump", meta = (AllowPrivateAccess = "true"))
	float DoubleJumpVelocity;
#pragma endregion

#pragma region WeaponSystem
protected:
	/** 무기를 교체하는 함수입니다. */
	void SwapWeapon();
#pragma endregion 
	
#pragma region TargetingSystem
protected:
	/** Target을 LockOn하는 함수입니다. */
	void ExecuteLockOnTarget();

protected:
	/** 일정범위 안이 LockOn이 가능한 Actor들을 탐지하여 Target으로 지정하고 플레이어의 카메라를 Target에게 고정하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetingSystem")
	class UPRTargetingSystemComponent* TargetingSystem;

public:
	/** TargetingSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTargetingSystemComponent* GetTargetingSystem() const
	{
		return TargetingSystem;
	}
#pragma endregion

#pragma region Effect
public:
	/**
	 * 비활성화 상태인 GhostTrail을 반환하는 함수입니다.
	 * 비활성화 상태인 GhostTrail이 없을 경우 GhostTrail을 새로 월드에 스폰하고 GhostTrailObjectPool에 저장합니다.
	 */
	class APRGhostTrail* GetGhostTrail();

	// /**
	//  * 활성화 상태인 GhostTrail을 비활성화하는 함수입니다.
	//  * @param EnableGhostTrail 비활성화할 활성화 상태인 GhostTrail입니다.
	//  */
	// void DisableGhostTrail(class APRGhostTrail* EnableGhostTrail);
	
private:
	/** GhostTrail을 비활성화 상태로 월드에 스폰하는 함수입니다. */
	class APRGhostTrail* SpawnGhostTrail() const;

	/**
	 * 입력받은 인자만큼 GhostTrail을 월드에 스폰하고 GhostTrailObjectPool에 저장하는 함수입니다.
	 * @param SpawnCount GhostTrail을 월드에 스폰할 횟수입니다.
	 */
	void InitializeGhostTrailObjectPool(int32 SpawnCount);
	
private:
	/** 캐릭터의 잔상(GhostTrail)을 표현하는 Actor 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayerCharacter|Effect", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APRGhostTrail> GhostTrail;

	/** GhostTrail을 월드에 스폰하는 횟수입니다. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PRPlayerCharacter|Effect", meta = (AllowPrivateAccess = "true"))
	int32 GhostTrailSpawnCount;
	
	/** GhostTrail ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPlayerCharacter|Effect", meta = (AllowPrivateAccess = "true"))
	TArray<class APRGhostTrail*> GhostTrailObjectPool;
#pragma endregion
	
#pragma region Dodge
protected:
	/** 캐릭터가 회피하는 함수입니다. */
	virtual void Dodge() override;
#pragma endregion 

#pragma region Attack
protected:
	/** 캐릭터가 공격하는 함수입니다. */
	virtual void Attack() override;
#pragma endregion

#pragma region AttackOverlapCollision
public:
	/**
	 * 비활성화 상태인 AttackOverlapCollision을 반환하는 함수입니다.
	 * 비활성화 상태인 AttackOverlapCollision이 없을 경우 AttackOverlapCollision을 새로 월드에 스폰하고
	 * AttackOverlapCollisionObjectPool에 저장합니다.
	 */
	class APROverlapCollision* GetAttackOverlapCollision();

private:
	/** AttackOverlapCollision을 비활성화 상태로 월드에 스폰하는 함수입니다. */
	class APROverlapCollision* SpawnAttackOverlapCollision();

	/**
	 * 입력받은 인자만큼 AttackOverlapCollision을 월드에 스폰하고 AttackOverlapCollisionObjectPool에 저장하는 함수입니다.
	 * @param SpawnCount AttackOverlapCollision을 월드에 스폰할 횟수입니다.
	 */
	void InitializeAttackOverlapCollision(int32 SpawnCount);

private:
	/** AttackOverlapCollision의 Debug를 출력을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRPlayerCharacter|AttackOverlapCollision", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugAttackOverlapCollision;
	
	/** 캐릭터의 잔상(GhostTrail)을 표현하는 Actor 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayerCharacter|AttackOverlapCollision", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APROverlapCollision> AttackOverlapCollision;
	
	/** AttackOverlapCollision을 월드에 스폰하는 횟수입니다. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PRPlayerCharacter|AttackOverlapCollision", meta = (AllowPrivateAccess = "true"))
	int32 AttackOverlapCollisionSpawnCount;

	/** AttackOverlapCollision ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPlayerCharacter|AttackOverlapCollision", meta = (AllowPrivateAccess = "true"))
	TArray<class APROverlapCollision*> AttackOverlapCollisionObjectPool;
#pragma endregion
};
