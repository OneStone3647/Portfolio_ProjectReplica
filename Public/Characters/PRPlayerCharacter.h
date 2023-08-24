// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PRPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPRTargetingSystemComponent;
class UPRTimeStopSystemComponent;
class UPREffectSystemComponent;
class UNiagaraSystem;
class APRAfterImage;
class APRBaseInteractableObject;
class APRBaseSkill;
class UPRTestEffectSystemComponent;

enum class EPRCommandSkill : uint8;

/**
 * 방향을 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRSkillPaletteSlot : uint8
{
	SkillPaletteSlot_Left			UMETA(Display = "LeftSlot"),
	SkillPaletteSlot_Up				UMETA(Display = "UpSlot"),
	SkillPaletteSlot_Right			UMETA(Display = "RightSlot"),
	SkillPaletteSlot_Down			UMETA(Display = "DownSlot")
};

/** UI 관련 Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboCountChangedDelegate, int32, NewComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenSkillPaletteDelegate, bool, bIsActivate);

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
	virtual void PostInitializeComponents() override;	// 액터에 속한 모든 컴포넌트의 세팅이 완료되면 호출되는 함수입니다. 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

#pragma region Debug
protected:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebug;
#pragma endregion 

#pragma region Camera
public:
	/** 카메라가 움직이는지 나타내는 함수입니다. */
	bool IsMoveCamera() const;

	/** ResetCamera에 관련된 것을 초기화하는 함수입니다. */
	void InitializeResetCameraTimeline();

	/** 카메라의 위치를 초기화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ActivateResetCamera();
	
	/**
	 * 입력받은 인자 값에 따라 월드를 흑백으로 설정합니다.
	 *
	 * @param bActivate true일 경우 월드를 흑백으로 false일 경우 원래대로 설정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ActivateWorldCameraMonochrome(bool bActivate);
	
protected:
	/**
	 * 마우스 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 *
	 * @param Value 마우스를 사용한 좌우 회전값입니다.
	 */
	void Turn(float Value);

	/**
	 * 마우스를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 *
	 * @param Value 마우스를 사용한 상하 회전값입니다.
	 */
	void LookUp(float Value);

	/**
	 * 게임패드를 사용할 때 사용하는 카메라 좌우 회전 함수입니다.
	 *
	 * @param Rate 게임패드를 사용한 좌우 회전값입니다.
	 */
	void TurnRate(float Rate);
	
	/**
	 * 게임패드를 사용할 때 사용하는 카메라 상하 회전 함수입니다.
	 *
	 * @param Rate 게임패드를 사용한 상하 회전값입니다.
	 */
	void LookUpRate(float Rate);

	/**
	 * 현재 카메라의 위치를 초기화하는 함수입니다.
	 *
	 * @param Value Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void ResetCamera(float Value);

protected:
	/** 캐릭터 뒤에 카메라를 배치하는 SpringArm입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	/** 캐릭터 뒤에서 따라오는 카메라입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	/** 카메라의 초기화한 위치를 나타내는 ArrowComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UArrowComponent* ResetCameraPositionArrow;
	
	/** 마우스를 사용할 때 카메라가 좌우 회전을 하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bTurnMoveCamera;

	/** 마우스를 사용할 때 카메라가 상하 회전을 하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bLookUpMoveCamera;

	/** 게임패드를 사용할 때 카메라가 좌우 회전을 하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bTurnRateMoveCamera;

	/** 게임패드를 사용할 때 카메라가 상하 회전을 하는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bLookUpRateMoveCamera;

	/** 카메라가 캐릭터 뒤에서 떨어져서 따라오는 거리입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseSpringArmLength;

	/** 카메라의 좌우 기본 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** 카메라의 상하 기본 회전 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	/** ResetCameraFloatCurve가 사용할 Callback 함수입니다. */
	FOnTimelineFloat ResetCameraTimelineProgress;

	/** 카메라의 위치를 초기화할 때 사용하는 Timeline입니다. */
	FTimeline ResetCameraTimeline;

	/** ResetCameraTimeline에서 사용하는 FloatCurve입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCurveFloat* ResetCameraFloatCurve;

	/** 화면을 흑백으로 나타내는 채도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector4 MonochromeColorSaturation;

	/** 화면을 흑백으로 나타내는 감마입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector4 MonochromeColorGamma;

public:
	/** FollowCamera를 반환하는 함수입니다. */
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion

#pragma region MovementInput
public:
	/**
	 * MoveForward 또는 MoveRight를 입력했는지 판별하는 함수입니다.
	 * @return MoveForward 또는 MoveRight를 입력했을 경우 true 입력하지 않았을 경우 false를 반환합니다.
	 */
	bool IsMoveInput() const;

	/** 입력 방향으로 캐릭터를 회전시키는 함수입니다. */
	void RotationInputDirection(bool bIsReverse = false);

	/** 캐릭터를 Target을 향해 자동으로 회전시키는 함수입니다. 캐릭터가 이동 입력이 있을 경우 이동 입력 방향으로 회전시킵니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void RotationAutoTargetDirection();

	/** InputComponent에서 MoveForward의 값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	float GetMoveForward() const;

	/** InputComponent에서 MoveRight의 값을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	float GetMoveRight() const;
	
protected:
	virtual void Jump() override;
	virtual void StopJumping() override;

	/** 더블 점프를 실행하는 함수입니다. */
	void DoubleJump();
	
	/**
	 * MoveForward 키를 입력할 경우 호출하는 함수입니다.
	 * 
	 * @param Value MoveForward의 입력값입니다.
	 */
	void MoveForward(float Value);

	/**
	 * MoveRight 키를 입력할 경우 호출하는 함수입니다.
	 * 
	 * @param Value MoveRight의 입력값입니다.
	 */
	void MoveRight(float Value);

	/**
	 * 게임패드의 아날로그 스틱의 대각선 입력을 보정한 값을 가져오는 함수입니다.
	 * 
	 * @param ForwardAxis MoveForward 값입니다.
	 * @param RightAxis MoveRight 값입니다.
	 * @param FixForwardAxis 대각선 입력을 보정한 MoveForward 값입니다.
	 * @param FixRightAxis 대각선 입력을 보정한 MoveRight 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void FixDiagonalGamepadValues(float ForwardAxis, float RightAxis, float& FixForwardAxis, float& FixRightAxis) const;

	/**
	 * 컨트롤러의 앞쪽 벡터와 오른쪽 벡터를 가져오는 함수입니다.
	 * 
	 * @param ForwardVector 컨트롤러의 앞쪽 벡터입니다.
	 * @param RightVector 컨트롤러의 오른쪽 벡터입니다.
	 */ 
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void GetControlForwardVectorAndRightVector(FVector& ForwardVector, FVector& RightVector) const;

	/**
	 * 방향에 따라 AddMovementInput을 호출하여 이동하는 함수입니다.
	 * 
	 * @param Direction 이동하는 방향입니다.
	 */
	virtual void AddPlayerMovementInput(EPRDirection Direction);

	/** 플레이어의 이동 입력 방향을 EPRDirection으로 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	EPRDirection GetMoveInputDirection() const;

	/**
	 * 캐릭터가 전력질주 하는 함수입니다.
	 * 
	 * @param bNewSprintInput true일 경우 캐릭터가 전력질주합니다. false일 경우 캐릭터는 달립니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void Sprint(bool bNewSprintInput);

	/** 캐릭터가 걷는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void Walk();

protected:
	/** 더블 점프 애니메이션 몽타주입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovementInput")
	UAnimMontage* DoubleJumpAnimMontage;
	
	// /** 더블 점프할 수 있는지 나타내는 변수입니다. */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInput")
	// bool bDoubleJumpable;

	/** 달리기 위한 Axis의 최고 입력 값입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInput")
	float MinRunInputAxis;

	/** 걷기 전환을 입력했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInput")
	bool bWalkToggleInput;

	/** 전력질주를 입력했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInput")
	bool bSprintInput;

	/** 회피를 입력했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementInput")
	bool bDodgeInput;
	
	/** 전력질주 하기 위한 입력키의 홀드 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementInput")
	float SprintableHoldTime;

	/** 전력질주 하기 위한 입력키의 홀드 시간이 지난 후 실행하기위한 TimerHandle입니다. */
	FTimerHandle SprintTimerHandle;
#pragma endregion

#pragma region TargetingSystem
protected:
	/** Target을 LockOn하는 함수입니다. */
	void ActivateLockOnTarget();

	/** Target을 변경준비를 하는 함수입니다. */
	void ReadyToChangeTarget(bool bIsReady);

private:
	/** 일정범위 안에 LockOn이 가능한 액터들을 탐지하여 Target으로 지정하고 플레이어의 카메라를 Target에게 고정하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetingSystem", meta = (AllowPrivateAccess = "true"))
	UPRTargetingSystemComponent* TargetingSystem;

	/** Target을 변경할 준비가 되었다는 것을 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", meta = (AllowPrivateAccess = "true"))
	bool bReadyToChangeTarget;
	
public:
	/** TargetingSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTargetingSystemComponent* GetTargetingSystem() const
	{
		return TargetingSystem;	
	}
#pragma endregion

#pragma region TimeStopSystem
private:
	/** 플레이어 캐릭터를 제외한 일정범위 안에 존재하는 Actor들을 일시정지하는 TimeStop을 실행하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TimeStopSystem", meta = (AllowPrivateAccess = "true"))
	UPRTimeStopSystemComponent* TimeStopSystem;

public:
	/** TimeStopSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRTimeStopSystemComponent* GetTimeStopSystem() const
	{
		return TimeStopSystem;
	}
#pragma endregion

#pragma region Interact
public:
	/** 입력받은 인자를 InteractableObjects Array에 추가하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void AddToInteractableObjects(AActor* NewInteractableObject);

	/** 입력받은 인자를 InteractableObjects Array에서 제거하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void RemoveToInteractableObjects(AActor* NewInteractableObject);
	
protected:
	/**
	 * 입력값에 따라 상호작용을 실행할 액터의 Index를 설정하는 함수입니다.
	 *
	 * @param Value 0보다 큰 경우 SelectInteractIndex를 1 증가하고 0보다 작은 경우 SelectInteractIndex를 1 감소합니다.
	 */
	void SelectInteraction(float Value);
	
	/** 상호작용을 실행하는 함수를 InputComponent에 바인딩할 함수입니다. */
	void ExecuteInteract();

private:
	/** 상호작용을 실행할 오브젝트들을 보관한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	TArray<class AActor*> InteractableObjects;
	
	/** 상호작용을 실행하기 위해 선택한 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	int32 SelectInteractIndex;

	/** 입력이 중복되지 않게 방지하는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	bool bResetInteractInput;
#pragma endregion 

#pragma region Dodge
protected:
	/** 회피를 실행하는 함수입니다. */
	virtual void Dodge() override;
#pragma endregion

#pragma region Dodge/Sprint
protected:
	/**
	 * 캐릭터가 회피와 전력질주를 하는 함수입니다.
	 * 첫 입력으로 회피를 하고 일정시간동안 입력키를 홀드하면 전력질주합니다.
	 *
	 * @param bNewSprintInput true일 경우 입력키를 홀드하는 동안 일정시간이 지난 후 캐릭터가 전력질주합니다. false일 경우 회피만합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRPlayerCharacter|Dodge/Sprint")
	void DodgeAndSprint(bool bNewSprintInput);
#pragma endregion

#pragma region CombatInput
protected:
	/** 일반 공격을 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CombatInput")
	virtual void NormalAttack();

	/** 강공격을 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CombatInput")
	virtual void ChargedAttack(FKey Key);
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
#pragma endregion

#pragma region Effect
	// DubleJump
private:
	/** 더블 점프할 때 사용하는 NiagaraSystem입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|DoubleJump", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DoubleJumpNiagaraEffect;

	// AfterImage
public:
	/** AfterImage를 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Effect|AfterImage")
	void ActivateAfterImage();
	
	/**
	 * AfterImageObjectPool에서 비활성화 상태인 AfterImage을 반환하는 함수입니다.
	 * 비활성화 상태인 AfterImage이 없을 경우 AfterImage을 새로 월드에 스폰하고 AfterImageObjectPool에 저장합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect|AfterImage")
	class APRAfterImage* GetAfterImage();

private:
	/** AfterImage을 비활성화 상태로 월드에 스폰하는 함수입니다. */
	class APRAfterImage* SpawnAfterImage() const;

	/**
	 * 입력받은 인자만큼 AfterImage을 월드에 스폰하고 AfterImageObjectPool에 저장하는 함수입니다.
	 * @param SpawnCount AfterImage을 월드에 스폰할 횟수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect|AfterImage")
	void InitializeAfterImageObjectPool(int32 SpawnCount);

private:
	/** 캐릭터의 잔상(AfterImage)을 표현하는 Actor의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|AfterImage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APRAfterImage> AfterImage;

	/** AfterImage을 월드에 스폰하는 횟수입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect|AfterImage", meta = (AllowPrivateAccess = "true"))
	int32 AfterImageInitSpawnCount;

	/** AfterImage의 ObjectPool입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect|AfterImage", meta = (AllowPrivateAccess = "true"))
	TArray<class APRAfterImage*> AfterImageObjectPool;
#pragma endregion

#pragma region SkillPalette
protected:
	/** 인자 값에 해당하는 전투 스킬을 실행하는 함수입니다. */
	void ActivateBattleSkill(EPRCommandSkill NewPRCommandSkill);
	
	/** 입력받은 인자에 따라 SkillPalette를 펼치고 접는 함수힙니다. */
	void OpenSkillPalette(bool bNewIsOpen);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRPlayerCharacter|SkillPalette")
	bool bIsOpenSkillPalette;
	
public:
	/** SkillPalette를 펼치거나 접을 때 호출하는 Delegate입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "PRPlayerCharacter|SkillPalette")
	FOnOpenSkillPaletteDelegate OnOpenSkillPaletteDelegate;
#pragma endregion

#pragma region ComboCount
public:
	/** ComboCount를 초기화하는 함수입니다. */
	void InitializeComboCount();
	
	/** ComboCount를 최신화하는 함수입니다. */
	void UpdateComboCount();

	/** 콤보 횟수를 초기화하는 시간의 남은 시간의 비율을 반환하는 함수입니다. */
	float GetComboCountResetTimeRatio() const;
	
protected:
	/** 마지막 콤보가 적용되고 더이상의 콤보가 없을 경우 설정한 시간이 지난 후 콤보 횟수를 초기화하는 TimerHandle입니다. */
	FTimerHandle ComboCountTimerHandle;

	/** 콤보 횟수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ComboCount")
	int32 ComboCount;

	/** 콤보 횟수를 초기화하는 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboCount")
	float ComboCountResetTime;

public:
	/** ComboCount가 변경되었을 때 실행하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "ComboCount")
	FOnComboCountChangedDelegate OnComboCountChangedDelegate;
#pragma endregion

#pragma region InGameHUD
private:
	/** InGameHUD을 초기화하는 함수입니다. */
	void InitializeInGameHUD();
#pragma endregion
};
