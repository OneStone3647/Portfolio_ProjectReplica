// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRTargetingSystemComponent.generated.h"

/**
 * 현재 Target에서 다음 Target이 존재하는 방향을 나타내느 enum 클래스입니다.
 */
UENUM(BlueprintType)
enum class EPRTargetDirection : uint8
{
	TargetDirection_Left		UMETA(DisplayName = "Left"),
	TargetDirection_Right		UMETA(DisplayName = "Right")
};

/**
 * 입력하는 장치를 나타내는 enum 클래스입니다.
 */
UENUM(BlueprintType)
enum class EPRInputMode : uint8
{
	InputMode_Mouse			UMETA(DisplayName = "Mouse"),
	InputMode_Gamepad		UMETA(DisplayName = "Gamepad")
};

/**
 * 일정범위 안에 LockOn이 가능한 액터들을 탐지하여 Target으로 지정하고 플레이어의 카메라를 Target에게 고정하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRTargetingSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRTargetingSystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Debug
private:
	/** 디버그를 최신화하는 함수입니다. */
	void UpdateDebug();

private:
	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	bool bDrawDebug;

	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSize;
#pragma endregion
	
public:
	/** Target을 LockOn하고 있는지 나타내는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRTargetingSystem")
	bool IsActivateLockOnTarget() const;

	/** Target의 LockOn을 실행하는 함수입니다. */
	void ActivateLockOnTarget();

	/** Target의 LockOn을 취소하는 함수입니다. */
	void CancelLockOnTarget();

	/**
	 * 입력하는 장치와 플레이어 카메라의 회전 값에 따라 LockOn하는 Target을 변경합니다.
	 *
	 * @param InputMode 입력장치입니다.
	 * @param TurnValue 플레이어 카메라의 회전 값입니다.
	 */
	void ChangeLockOnTargetForTurnValue(EPRInputMode InputMode, float TurnValue);
	
private:
	/** LockOnTarget을 향해 카메라를 고정하는 것을 최신화하는 함수입니다. */
	void UpdateCameraLock();

	/** LockOnTarget에 카메라를 고정하는 함수입니다. */
	void EnableCameraLock();

	/** LockOnTarget에 고정된 카메라의 고정을 해제하는 함수입니다. */
	void DisableCameraLock();

	/** 입력받은 인자를 LockedOnTarget으로 설정하는 함수입니다. */
	void SetLockedOnTarget(AActor* NewTarget);

	/** 마우스로 플레이어 카메라의 회전 값에 따라 LockOn하는 Target을 변경하는 함수입니다. */
	void ChangeLockOnTargetWithMouse(const float& TurnValue);

	/** 게임패드로 플레이어 카메라의 회전 값에 따라 LockOn하는 Target을 변경하는 함수입니다. */
	void ChangeLockOnTargetWithGamepad(const float& TurnRate);

	/** LockOnTarget으로 지정이 가능한 액터들을 탐색하고 탐색한 액터들 중 화면의 중앙에 가장 가깝게 존재하는 액터를 찾아 반환하는 함수입니다. */
	AActor* FindTarget() const;

	/** LockOnTarget으로 지정이 가능한 액터들을 탐색하여 Array에 저장하고 반환하는 함수입니다. */
	TArray<AActor*> SearchTargetableActors() const;

	/**
	 * 현재 LockOnTarget을 기준으로 입력된 방향에 있는 새로운 Target을 찾아 반환하는 함수입니다.
	 *
	 * @param NewTargetDirection 새로운 Target을 찾을 방향입니다.
	 */
	AActor* FindDirectionalTarget(EPRTargetDirection NewTargetDirection);

	/**
	 * 인자로 받은 액터가 플레이어 카메라에서 Target을 기준으로 왼쪽에 있는지 오른쪽에 있는지 반환하는 함수입니다.
	 *
	 * @param NewTargetableActor 위치를 파악할 액터입니다.
	 */
	EPRTargetDirection WhichSideOfTarget(AActor* NewTargetableActor) const;

	/**
	 * 내적을 기준으로 새로운 Target을 가져오는 함수입니다.
	 *
	 * @param TargetableActors Target으로 지정이 가능한 액터들입니다.
	 * @param bIsLargestDot true일 경우 가장 큰 내적, false일 경우 가장 작은 내적을 기준으로 사용합니다. 
	 */
	AActor* GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot);

	/**
	 * 플레이어 캐릭터에서 Target으로 향하는 벡터와 플레이어 캐릭터에서 인자로 받은 액터로 향하는 벡터와 내적을 계산하는 함수입니다.
	 *
	 * @param NewTargetableActor 벡터를 비교할 액터입니다. 
	 */
	float CalculateDotProductToTarget(const AActor& NewTargetableActor) const;

	/**
	 * 인자로 받은 액터의 화면상의 위치와 해당 액터가 화면에 존재하는지를 반환하는 함수입니다.
	 *
	 * @param SearchActor 화면상의 위치를 찾을 액터입니다.
	 * @param SearchActorPositionOnScreen 인자로 받은 액터의 화면상의 위치입니다.
	 * @param bIsSearchActorOnScreen 인자로 받은 액터가 화면상에 존재하는지를 나타냅니다. 
	 */
	void GetScreenPositionOfActor(const AActor& SearchActor, FVector2D& SearchActorPositionOnScreen, bool& bIsSearchActorOnScreen) const;

	/**
	 * 화면상의 액터의 위치가 화면에 존재하는지 판별하는 함수입니다.
	 *
	 * @param ActorPositionOnScreen 화면상의 액터의 위치입니다.
	 * @param ScreenRatio 화면 크기의 비율을 나타내는 값입니다. 0.0f 이하 또는 1.0f 이상일 경우 화면의 최대크기를 사용합니다.
	 *						예를 들어 0.8f일 경우 중앙에서 화면의 상하좌우를 0.8f 비율만큼 줄인 크기를 사용합니다.
	 * @return 액터의 위치가 화면에 존재하는지 나타냅니다.
	 */
	bool IsOnScreen(const FVector2D& ActorPositionOnScreen, float ScreenRatio = 1.0f) const;

	/**
	 * 인자로 받은 액터를 바라보는 회전 보간 값을 계산하는 함수입니다.
	 *
	 * @param InterpToTarget 바라볼 액터입니다. 
	 */
	FRotator CalculateInterpToTarget(const AActor& InterpToTarget) const;

private:
	/** Target을 LockOn하고 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTargetingSystem", meta = (AllowPrivateAccess = "true"))
	bool bActivateLockOnTarget;

	/** LockOn된 Target입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTargetingSystem", meta = (AllowPrivateAccess = "true"))
	AActor* LockedOnTarget;

	/** Target으로 지정이 가능한 액터들을 탐색하는 최대 탐색 범위입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTargetingSystem", meta = (AllowPrivateAccess = "true"))
	float MaxSearchTargetalbeDistance;

	/** 동적 CameraLock을 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTargetingSystem", meta = (AllowPrivateAccess = "true"))
	bool bActivateDynamicCameraLock;

	/** 동적으로 CameraLock을 실행을 중지하는 화면의 비율입니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PTTargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float StopDynamicCameraLockScreenRatio;

	/** Target을 LockOn할 때 Target을 바라보는 카메라의 보간속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRTargetingSystem", meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	/**
	 * 마지막으로 Target을 변경한 시간입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "PRTargetingSystem|MouseInput", meta = (AllowPrivateAccess = "true"))
	float LastTimeChangeTarget;

	/**
	 * 마지막으로 Target을 변경한 이후의 시간입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "PRTargetingSystem|MouseInput", meta = (AllowPrivateAccess = "true"))
	float TimeSinceChangeTarget;

	/**
	 * Target을 변경하기 위해 필요한 마우스의 최소 회전 입력 값입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRTargetingSystem|MouseInput", meta = (AllowPrivateAccess = "true"))
	float MinMouseValueToChangeTarget;

	/**
	 * 다른 Target으로 변경하기 전에 딜레이를 주는 시간입니다.
	 * 너무 빠르게 다른 Target으로 변경하는 것을 방지합니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRTargetingSystem|MouseInput", meta = (AllowPrivateAccess = "true"))
	float MinDelayTimeToChangeTarget;

	/**
	 * 마지막으로 Target을 변경한 후 게임패드의 아날로그 스틱이 중립 상태가 되었는지 나타내는 변수입니다.
	 * 게임패드의 아날로그 스틱이 중립일 경우 true로 설정합니다.
	 * 게임패드로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "PRTargetingSystem|GamepadInput", meta = (AllowPrivateAccess = "true"))
	bool bAnalogNeutralSinceLastSwitchTarget;

	/**
	 * Target을 변경하기 위해 필요한 아날로그 스틱의 최소 입력 값입니다.
	 * 게임패드로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRTargetingSystem|GamepadInput", meta = (AllowPrivateAccess = "true"))
	float MinAnalogValueToChangeTarget;

	// /** LockedOnTarget의 왼쪽에 존재하는 TargetableActor들을 보관한 Array입니다. */
	// TArray<AActor*> LeftTargetableActors;
	//
	// /** LockedOnTarget의 오른쪽에 존재하는 TargetableActor들을 보관한 Array입니다. */
	// TArray<AActor*> RightTargetableActors;
	
public:
	/** LockedOnTarget을 반환하는 함수입니다. */
	AActor* GetLockedOnTarget() const;

	/** MaxSearchTargetalbeDistance을 반환하는 함수입니다. */
	float GetMaxSearchTargetalbeDistance() const;

	/** 입력받은 인자 값으로 bActivateDynamicCameraLock을 설정하는 함수입니다. */
	void SetActivateDynamicCameraLock(bool bNewActivateDynamicCameraLock);
};
