// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRPlayerCharacter.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Components/PRTimeStopSystemComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "ProjectReplicaGameMode.h"
#include "PRPlayerController.h"
#include "Components/ArrowComponent.h"
#include "Effect/PRNiagaraEffect.h"
#include "Interfaces/PRInteractInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/PRComboCountWidget.h"
#include "Widgets/PRInGameHUD.h"
#include "Widgets/PRInteractWidget.h"
#include "Skills/PRBaseSkill.h"
#include "Weapons/PRBaseWeapon.h"
#include "Components/SphereComponent.h"
#include "CinematicCamera/Public/CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"

APRPlayerCharacter::APRPlayerCharacter()
{
	// Debug
	bDebug = false;

	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerCharacter"));
	
	// Camera
	SpringArm = nullptr;
	FollowCamera = nullptr;
	
	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	BaseSpringArmLength = 400.0f;
	SpringArm->TargetArmLength = BaseSpringArmLength;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 200.0f);
	SpringArm->bUsePawnControlRotation = true;										// 컨트롤러를 기준으로 SpringArm을 회전합니다.
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 10.0f;
	SpringArm->CameraRotationLagSpeed = 20.0f;

	// FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;									// 카메라가 SpringArm을 기준으로 회전하지 않습니다.
	FollowCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	// SkillCamera
	SkillCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("SkillCamera"));
	SkillCamera->SetupAttachment(RootComponent);
	SkillCamera->SetConstraintAspectRatio(false);

	// SceneCapture
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(FollowCamera);
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;

	// ResetCameraPosition
	ResetCameraPositionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ResetCameraPositionArrow"));
	ResetCameraPositionArrow->SetupAttachment(RootComponent);
	
	// Camera
	bLockCamera = false;
	bTurnMoveCamera = false;
	bLookUpMoveCamera = false;
	bTurnRateMoveCamera = false;
	bLookUpRateMoveCamera = false;
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	ResetCameraFloatCurve = nullptr;
	MonochromeFloatCurve = nullptr;
	MonochromeMode = EPRCameraPostProcessMaterial::CameraPostProcessMaterial_None;

	// MovementInput
	bLockMovementInput = false;
	DoubleJumpAnimMontage = nullptr;
	MinRunInputAxis = 0.42f;
	bWalkToggleInput = false;
	bSprintInput = false;
	bDodgeInput = false;
	SprintableHoldTime = 0.3f;

	// ObjectPoolSystem
	// GetObjectPoolSystem()->SetIgnoreTimeStop(true);

	// TargetingSystem
	TargetingSystem = CreateDefaultSubobject<UPRTargetingSystemComponent>(TEXT("TargetingSystem"));
	bReadyToChangeTarget = false;

	// TimeStopSystem
	// TimeStopSystem = CreateDefaultSubobject<UPRTimeStopSystemComponent>(TEXT("TimeStopSystem"));

	// Dodge
	ExtremeDodgeArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExtremeDodgeArea"));
	ExtremeDodgeArea->SetCollisionProfileName("ExtremeDodgeArea");
	ExtremeDodgeArea->SetupAttachment(RootComponent);
	DeactivateExtremeDodgeArea();

	// Interact
	InteractableObjects.Empty();
	SelectInteractIndex = 0;
	bResetInteractInput = false;
	
	// Effect
	SignatureEffectColor = FLinearColor(20.0f, 15.0f, 200.0f, 1.0f);
	// Effect|DoubleJump
	DoubleJumpNiagaraEffect = nullptr;
	// Effect|AfterImage
	AfterImage = nullptr;
	
	// SkillPalette
	bIsOpenSkillPalette = false;

	// ComboCount
	bActivateComboCount = false;
	ComboCountResetRemaining = 0.0f;
	ComboCountResetElapsed = 0.0f;
	ComboCount = 0;
	ComboCountResetTime = 3.0f;
}

void APRPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// TimeStopSystem
	// GetTimeStopSystem()->OnDeactivateTimeStop.AddDynamic(this, &APRPlayerCharacter::OnDeactivateTimeStop);
	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsValid(PRGameMode) == true)
	{
		PRGameMode->GetTimeStopSystem()->OnDeactivateTimeStop.AddDynamic(this, &APRPlayerCharacter::APRPlayerCharacter::OnDeactivateTimeStop);
	}
}

void APRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Camera
	InitializeResetCameraTimeline();
	InitializeMonochromeModeTimeline();

	// InGameHUD
	// 컨트롤러에서 뷰포트에 위젯을 추가하는 것이 캐릭터의 BeginPlay보다 먼저 실행되므로
	// 캐릭터가 시작되면서 HUD에 필요한 정보를 초기화합니다.
	InitializeInGameHUD();

	// WeaponSystem
	GetWeaponSystem()->InitializeWeaponInventory();

	// StateSystem
	if(GetWeaponSystem()->GetWeaponInventory().Num() != 0)
	{
		GetStateSystem()->SetActionable(EPRAction::Action_Attack, true);
	}

	// //EffectSystem
	// GetEffectSystem()->SetIgnoreTimeStop(true);
}

void APRPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera
	ResetCameraTimeline.TickTimeline(DeltaTime);
	MonochromeModeTimeline.TickTimeline(DeltaTime);

	// ComboCount
	UpdateComboCount(DeltaTime);
}

void APRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APRPlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APRPlayerCharacter::StopJumping);
	
	// 마우스를 사용한 카메라 회전
	PlayerInputComponent->BindAxis("Turn", this, &APRPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APRPlayerCharacter::LookUp);

	// 게임패드를 사용한 카메라 회전
	PlayerInputComponent->BindAxis("TurnRate", this, &APRPlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APRPlayerCharacter::LookUpRate);

	PlayerInputComponent->BindAxis("MoveForward", this, &APRPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APRPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &APRPlayerCharacter::Walk);

	PlayerInputComponent->BindAction("LockOnTarget", IE_Pressed, this, &APRPlayerCharacter::ActivateLockOnTarget);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("ReadyToChangeTarget", IE_Pressed, this ,&APRPlayerCharacter::ReadyToChangeTarget, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("ReadyToChangeTarget", IE_Released, this ,&APRPlayerCharacter::ReadyToChangeTarget, false);

	PlayerInputComponent->BindAxis("SelectInteraction", this, &APRPlayerCharacter::SelectInteraction);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APRPlayerCharacter::ExecuteInteract);

	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Sprint", IE_Pressed, this ,&APRPlayerCharacter::Sprint, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Sprint", IE_Released, this ,&APRPlayerCharacter::Sprint, false);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APRPlayerCharacter::Dodge);

	// 원신식 회피/달리기
	// PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Dodge/Sprint", IE_Pressed, this ,&APRPlayerCharacter::DodgeAndSprint, true);
	// PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Dodge/Sprint", IE_Released, this ,&APRPlayerCharacter::DodgeAndSprint, false);
	
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("OpenSkillPalette", IE_Pressed, this ,&APRPlayerCharacter::OpenSkillPalette, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("OpenSkillPalette", IE_Released, this ,&APRPlayerCharacter::OpenSkillPalette, false);

	PlayerInputComponent->BindAction<TDelegate<void(EPRCommandSkill)>>("ActivateFirstBattleSkill", IE_Pressed, this, &APRPlayerCharacter::ActivateBattleSkill, EPRCommandSkill::CommandSkill_FirstBattleSkill);
	PlayerInputComponent->BindAction<TDelegate<void(EPRCommandSkill)>>("ActivateSecondBattleSkill", IE_Pressed, this, &APRPlayerCharacter::ActivateBattleSkill, EPRCommandSkill::CommandSkill_SecondBattleSkill);
	PlayerInputComponent->BindAction<TDelegate<void(EPRCommandSkill)>>("ActivateThirdBattleSkill", IE_Pressed, this, &APRPlayerCharacter::ActivateBattleSkill, EPRCommandSkill::CommandSkill_ThirdBattleSkill);
	PlayerInputComponent->BindAction<TDelegate<void(EPRCommandSkill)>>("Ultimate", IE_Pressed, this, &APRPlayerCharacter::ActivateBattleSkill, EPRCommandSkill::CommandSkill_Ultimate);
}

void APRPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	GetStateSystem()->SetActionable(EPRAction::Action_DoubleJump, true);
}

float APRPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,	AActor* DamageCauser)
{
	UPRBaseSkill* ExtremeDodge = GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_ExtremeDodge);
	if(ExtremeDodge != nullptr && ExtremeDodge->IsCanActivateSkill() == true)
	{
		GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_ExtremeDodge)->ActivateSkill();
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

#pragma region Camera
bool APRPlayerCharacter::bIsLockCamera() const
{
	return bLockCamera;
}

void APRPlayerCharacter::SetLockCamera(bool bNewLockCamera)
{
	bLockCamera = bNewLockCamera;
}

bool APRPlayerCharacter::IsMoveCamera() const
{
	return bTurnMoveCamera || bLookUpMoveCamera || bTurnRateMoveCamera || bLookUpRateMoveCamera;
}

void APRPlayerCharacter::ActivateResetCamera()
{
	ResetCameraTimeline.PlayFromStart();
}

void APRPlayerCharacter::InitializeMonochromeMode()
{
	SetPostProcessMaterialWeight(EPRCameraPostProcessMaterial::CameraPostProcessMaterial_TimeStop, 0.0f);
	SetPostProcessMaterialWeight(EPRCameraPostProcessMaterial::CameraPostProcessMaterial_BlackAndWhite, 0.0f);
}

void APRPlayerCharacter::SetPostProcessMaterialWeight(EPRCameraPostProcessMaterial CameraPostProcessMaterial, float Value)
{
	if(GetFollowCamera() != nullptr)
	{
		int32 MaterialIndex = static_cast<int32>(CameraPostProcessMaterial);
		FPostProcessSettings NewPostProcessSettings = GetFollowCamera()->PostProcessSettings;

		// 설정할 PostProcessMaterial이 존재할 경우
		if(NewPostProcessSettings.WeightedBlendables.Array.IsValidIndex(MaterialIndex) == true)
		{
			NewPostProcessSettings.WeightedBlendables.Array[MaterialIndex].Weight = Value;

			// 변경된 설정을 적용합니다.
			GetFollowCamera()->PostProcessSettings = NewPostProcessSettings;
		}
	}
}

void APRPlayerCharacter::ActivateMonochromeMode(EPRCameraPostProcessMaterial CameraPostProcessMaterial, bool bUseLerp, bool bReverse)
{
	MonochromeMode = CameraPostProcessMaterial;
	
	// 선형보간을 사용하며 선형보간에 필요한 Curve가 존재할 떄
	if(bUseLerp && MonochromeFloatCurve != nullptr)
	{
		if(bReverse)
		{
			MonochromeModeTimeline.ReverseFromEnd();
		}
		else
		{
			MonochromeModeTimeline.PlayFromStart();
		}
	}
	// 선형보간을 사용하지 않거나 선형보간에 필요한 Curve가 존재하지 않을 때
	else
	{
		if(bReverse)
		{
			SetPostProcessMaterialWeight(MonochromeMode, 0.0f);
		}
		else
		{
			SetPostProcessMaterialWeight(MonochromeMode, 1.0f);
		}
	}
}

void APRPlayerCharacter::DeactivateMonochromeMode(EPRCameraPostProcessMaterial CameraPostProcessMaterial)
{
	SetPostProcessMaterialWeight(CameraPostProcessMaterial, 0.0f);
	MonochromeMode = EPRCameraPostProcessMaterial::CameraPostProcessMaterial_None;
}

void APRPlayerCharacter::ActivateSkillCamera()
{
	SkillCamera->Activate();
	FollowCamera->Deactivate();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetViewTargetWithBlend(this, 0.2f, EViewTargetBlendFunction::VTBlend_EaseInOut, 10.0f, false);
}

void APRPlayerCharacter::DeactivateSkillCamera()
{
	SkillCamera->Deactivate();
	FollowCamera->Activate();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetViewTargetWithBlend(this, 0.2f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f, false);
}

void APRPlayerCharacter::Turn(float Value)
{
	if(Value != 0.0f && bIsLockCamera() == false)
	{
		TargetingSystem->SetActivateDynamicCameraLock(false);

		bTurnMoveCamera = true;
		
		if(TargetingSystem->IsActivateLockOnTarget() == true && bReadyToChangeTarget)
		{
			TargetingSystem->ChangeLockOnTargetForTurnValue(EPRInputMode::InputMode_Mouse, Value);
		}
		else
		{
			AddControllerYawInput(Value);
		}

		// AddControllerYawInput(Value);
	}
	else
	{
		bTurnMoveCamera = false;
	}
}

void APRPlayerCharacter::LookUp(float Value)
{
	if(Value != 0.0f && bIsLockCamera() == false)
	{
		TargetingSystem->SetActivateDynamicCameraLock(false);

		bLookUpMoveCamera = true;
		
		if(!bReadyToChangeTarget)
		{
			AddControllerPitchInput(Value);
		}
		
		// AddControllerPitchInput(Value);
	}
	else
	{
		bLookUpMoveCamera = false;
	}
}

void APRPlayerCharacter::TurnRate(float Rate)
{
	if(Rate != 0.0f && bIsLockCamera() == false)
	{
		TargetingSystem->SetActivateDynamicCameraLock(false);

		bTurnRateMoveCamera = true;
		
		if(TargetingSystem->IsActivateLockOnTarget() == true && bReadyToChangeTarget)
		{
			TargetingSystem->ChangeLockOnTargetForTurnValue(EPRInputMode::InputMode_Gamepad, Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		}
		else
		{
			AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		}

		// AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		bTurnRateMoveCamera = false;
	}
}

void APRPlayerCharacter::LookUpRate(float Rate)
{
	if(Rate != 0.0f && bIsLockCamera() == false)
	{
		TargetingSystem->SetActivateDynamicCameraLock(false);

		bLookUpRateMoveCamera = true;
		
		if(!bReadyToChangeTarget)
		{
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		}

		// AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		bLookUpRateMoveCamera = false;
	}
}

void APRPlayerCharacter::ResetCamera(float Value)
{
	if(ResetCameraFloatCurve != nullptr)
	{
		const FRotator ControlRotation = FRotator(GetController()->GetControlRotation().Pitch, GetController()->GetControlRotation().Yaw, 0.0f);
		const FRotator ResetCameraPositionArrowRotation = FRotator(ResetCameraPositionArrow->GetComponentRotation().Pitch, ResetCameraPositionArrow->GetComponentRotation().Yaw, 0.0f);
		FRotator NewControlRotation = UKismetMathLibrary::REase(ControlRotation, ResetCameraPositionArrowRotation, Value, true, EEasingFunc::EaseInOut);
		
		GetController()->SetControlRotation(NewControlRotation);
	}
}

void APRPlayerCharacter::InitializeResetCameraTimeline()
{
	if(ResetCameraFloatCurve != nullptr)
	{
		float CurveMinTime = 0.0f;
		float CurveMaxTime = 0.0f;

		if(ResetCameraTimelineProgress.IsBound() == false)
		{
			// Callback 함수에서 사용할 함수를 바인딩합니다.
			ResetCameraTimelineProgress.BindUFunction(this, FName("ResetCamera"));

			// Timeline에 Curve를 추가합니다.
			ResetCameraTimeline.AddInterpFloat(ResetCameraFloatCurve, ResetCameraTimelineProgress, NAME_None, TEXT("ResetCamera"));
		}
		else
		{
			// Timeline을 초기값으로 초기화합니다.
			ResetCameraTimeline.SetFloatCurve(ResetCameraFloatCurve, TEXT("ResetCamera"));
		}
		
		ResetCameraFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
		ResetCameraTimeline.SetPlayRate(1.0f);
		ResetCameraTimeline.SetTimelineLength(CurveMaxTime);
	}
}

void APRPlayerCharacter::InitializeMonochromeModeTimeline()
{
	if(MonochromeFloatCurve != nullptr)
	{
		float CurveMinTime = 0.0f;
		float CurveMaxTime = 0.0f;

		if(MonochromeModeTimelineProgress.IsBound() == false)
		{
			// Callback 함수에서 사용할 함수를 바인딩합니다.
			MonochromeModeTimelineProgress.BindUFunction(this, FName("MonochromeModeLerp"));

			// Timeline에 Curve를 추가합니다.
			MonochromeModeTimeline.AddInterpFloat(MonochromeFloatCurve, MonochromeModeTimelineProgress, NAME_None, TEXT("MonochromeModeLerp"));
		}
		else
		{
			// Timeline을 초기값으로 초기화합니다.
			MonochromeModeTimeline.SetFloatCurve(MonochromeFloatCurve, TEXT("MonochromeModeLerp"));
		}
		
		MonochromeFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
		MonochromeModeTimeline.SetPlayRate(1.0f);
		MonochromeModeTimeline.SetTimelineLength(CurveMaxTime);
	}
}

void APRPlayerCharacter::MonochromeModeLerp(float Value)
{
	SetPostProcessMaterialWeight(MonochromeMode, Value);
}
#pragma endregion 

#pragma region MovementInput
bool APRPlayerCharacter::IsLockMovementInput() const
{
	return bLockMovementInput;
}

void APRPlayerCharacter::SetLockMovementInput(bool bNewLockMovementInput)
{
	bLockMovementInput = bNewLockMovementInput;
}

bool APRPlayerCharacter::IsMoveInput() const
{
	return (GetMoveForward() != 0.0f || GetMoveRight() != 0.0f) ? true : false;
}

void APRPlayerCharacter::RotationInputDirection(bool bIsReverse)
{
	const float MoveForward = GetMoveForward();
	const float MoveRight = GetMoveRight();
	FVector InputVector = FVector(MoveForward, MoveRight, 0.0f);
	
	if(bIsReverse)
	{
		InputVector = FVector(MoveForward * -1.0f, MoveRight * -1.0f, 0.0f);
	}

	const FRotator InputRotator = UKismetMathLibrary::MakeRotFromX(InputVector);
	float InputDirectionYaw = 0.0f;

	// 입력이 없을 경우 액터를 기준으로 회전합니다.
	if(MoveForward == 0.0f && MoveRight == 0.0f)
	{
		InputDirectionYaw = InputRotator.Yaw + GetActorRotation().Yaw;
	}
	else
	{
		InputDirectionYaw = InputRotator.Yaw + GetControlRotation().Yaw;
	}

	const FRotator InputDirectionRotator = FRotator(0.0f, InputDirectionYaw, 0.0f);
	SetActorRotation(InputDirectionRotator);
}

void APRPlayerCharacter::RotationAutoTargetDirection()
{
	// LockOnTarget이 활성화 되고 이동 입력을 하지 않았을 경우 Target을 향해 캐릭터를 회전시킵니다.
	if(GetTargetingSystem()->IsActivateLockOnTarget() == true && IsMoveInput() == false)
	{
		const float LookAtRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetTargetingSystem()->GetLockedOnTarget()->GetActorLocation()).Yaw;
		SetActorRotation(FRotator(0.0f, LookAtRotationYaw, 0.0f));
	}
	// 캐릭터를 이동 입력 방향으로 회전시킵니다.
	else
	{
		RotationInputDirection();
	}
}

float APRPlayerCharacter::GetMoveForward() const
{
	return GetInputAxisValue("MoveForward");
}

float APRPlayerCharacter::GetMoveRight() const
{
	return GetInputAxisValue("MoveRight");
}

void APRPlayerCharacter::Jump()
{
	if(IsLockMovementInput() == false)
	{
		Super::Jump();

		// 캐릭터가 지상에 있을 경우 현재 재생중인 PRAnimMontage를 정지합니다.
		if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_Grounded) == true)
		{
			GetAnimSystem()->StopPRAnimMontage();
		}
		// PRAnimMontage 정지로 인한 State를 초기화합니다.
		GetStateSystem()->SetActionable(EPRAction::Action_Move, true);
		GetStateSystem()->SetActionable(EPRAction::Action_Attack, true);
		GetStateSystem()->SetActionable(EPRAction::Action_Dodge, true);
		// GetWeaponSystem()->SetEquippedWeaponGroupAttackPRAnimMontageIndex(0);

		// 현재 사용하는 무기가 발도 상태면 무기를 납도합니다.
		if(IsValid(GetWeaponSystem()->GetEquippedWeapon()) == true && GetWeaponSystem()->GetEquippedWeapon()->IsDraw() == true)
		{
			GetWeaponSystem()->GetEquippedWeapon()->Sheath();
		}

		// 일반 공격 Index를 초기화합니다.
		InitializePlayNormalAttackIndex();
	
		if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true
			&& GetStateSystem()->IsActionable(EPRAction::Action_DoubleJump) == true)
		{
			DoubleJump();
		}
	}
}

void APRPlayerCharacter::StopJumping()
{
	Super::StopJumping();	
}

void APRPlayerCharacter::DoubleJump()
{;
	// bDoubleJumpable = false;
	GetStateSystem()->SetActionable(EPRAction::Action_DoubleJump, false);
	GetStateSystem()->SetCanCancelAction(false);

	// 현재 사용하는 무기가 발도 상태면 무기를 납도합니다.
	if(IsValid(GetWeaponSystem()->GetEquippedWeapon()) == true && GetWeaponSystem()->GetEquippedWeapon()->IsDraw() == true)
	{
		GetWeaponSystem()->GetEquippedWeapon()->Sheath();
	}

	// 일반 공격 Index를 초기화합니다.
	InitializePlayNormalAttackIndex();
	
	const float CurrentSpeed = GetVelocity().Size2D() * 0.8f;		// 떨어지는 속도는 제외합니다.
	const float MoveForward = GetMoveForward();
	const float MoveRight = GetMoveRight();
	FVector ForwardVector = FVector::ZeroVector;
	FVector RightVector = FVector::ZeroVector;
	GetControlForwardVectorAndRightVector(ForwardVector, RightVector);
	
	GetMovementSystem()->ActivateAerial(true);
	RotationInputDirection();
	PlayAnimMontage(DoubleJumpAnimMontage);

	// Effect Spawn
	if(DoubleJumpNiagaraEffect)
	{
		// Z0 애니메이션이 없기 때문에 중앙을 나타내는 root 소켓의 XY값에 발의 높이(Z)를 더하여 사용하였습니다.
		const FVector CenterLocation = GetMesh()->GetSocketLocation(FName("root"));
		const FVector LeftFootLocation = GetMesh()->GetSocketLocation(FName("foot_l"));
		const FVector RightFootLocation = GetMesh()->GetSocketLocation(FName("foot_r"));
		const FVector NewSpawnEffectLocation = FVector(CenterLocation.X, CenterLocation.Y, UKismetMathLibrary::Min(LeftFootLocation.Z, RightFootLocation.Z));
		// UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DoubleJumpNiagaraEffect, NewSpawnEffectLocation);
		// SpawnNiagaraComponent->SetVariableLinearColor("EffectColor", SignatureEffectColor);

		UPRNiagaraEffect* DoubleJumpEffect = GetEffectSystem()->SpawnNiagaraEffectAtLocation(DoubleJumpNiagaraEffect, NewSpawnEffectLocation);
		if(DoubleJumpEffect != nullptr)
		{
			DoubleJumpEffect->GetNiagaraEffect()->SetVariableLinearColor("EffectColor", SignatureEffectColor);
		}

		// Z0 애니메이션일 경우
		// UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DoubleJumpNiagaraEffect, GetMesh()->GetSocketLocation(FName("root")));
	}

	const FVector NewForwardVector = (ForwardVector * MoveForward).GetSafeNormal() * CurrentSpeed;
	const FVector NewRightVector = (RightVector * MoveRight).GetSafeNormal() * CurrentSpeed;
	const FVector Velocity = NewForwardVector + NewRightVector + FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity);
	LaunchCharacter(Velocity, true, true);
	GetMovementSystem()->SetInitializeSpeedWhenJump(true);
	GetMovementSystem()->ActivateAerial(false);
	GetStateSystem()->SetActionable(EPRAction::Action_Dodge, true);
}

void APRPlayerCharacter::MoveForward(float Value)
{
	if(Controller != nullptr && Value != 0.0f && IsLockMovementInput() == false
		&& GetStateSystem()->IsActionable(EPRAction::Action_Move) == true)
	{
		if(GetAnimSystem()->IsPlayPRAnimMontage() == true && GetStateSystem()->IsCanCancelAction() == true)
		{
			GetAnimSystem()->StopPRAnimMontage();
		}
		
		AddPlayerMovementInput(EPRDirection::Direction_Forward);
	}
}

void APRPlayerCharacter::MoveRight(float Value)
{
	if(Controller != nullptr && Value != 0.0f && IsLockMovementInput() == false
		&& GetStateSystem()->IsActionable(EPRAction::Action_Move) == true)
	{
		if(GetAnimSystem()->IsPlayPRAnimMontage() == true && GetStateSystem()->IsCanCancelAction() == true)
		{
			GetAnimSystem()->StopPRAnimMontage();
		}
		
		AddPlayerMovementInput(EPRDirection::Direction_Right);
	}
}

void APRPlayerCharacter::FixDiagonalGamepadValues(float ForwardAxis, float RightAxis, float& FixForwardAxis, float& FixRightAxis) const
{
	const float NewForwardAxis = ForwardAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
																				FVector2D(1.0f, 1.2f),
																				FMath::Abs(RightAxis));
	const float NewRightAxis = RightAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f),
																			FVector2D(1.0f, 1.2f),
																			FMath::Abs(ForwardAxis));

	FixForwardAxis = FMath::Clamp(NewForwardAxis, -1.0f, 1.0f);
	FixRightAxis = FMath::Clamp(NewRightAxis, -1.0f, 1.0f);
}

void APRPlayerCharacter::GetControlForwardVectorAndRightVector(FVector& ForwardVector, FVector& RightVector) const
{
	const FRotator NewControlRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	
	ForwardVector = UKismetMathLibrary::GetForwardVector(NewControlRotation);
	RightVector = UKismetMathLibrary::GetRightVector(NewControlRotation);
}

void APRPlayerCharacter::AddPlayerMovementInput(EPRDirection Direction)
{
	// 현재 사용하는 무기가 발도 상태면 무기를 납도합니다.
	if(IsValid(GetWeaponSystem()->GetEquippedWeapon()) == true && GetWeaponSystem()->GetEquippedWeapon()->IsDraw() == true)
	{
		GetWeaponSystem()->GetEquippedWeapon()->Sheath();
	}
	
	FVector ForwardVector = FVector::ZeroVector;
	FVector RightVector = FVector::ZeroVector;
	GetControlForwardVectorAndRightVector(ForwardVector, RightVector);

	float MoveForward = 0.0f;
	float MoveRight = 0.0f;
	FixDiagonalGamepadValues(GetMoveForward(), GetMoveRight(), MoveForward, MoveRight);
	
	switch(Direction)
	{
	case EPRDirection::Direction_Forward:
		AddMovementInput(ForwardVector, MoveForward);
		break;
	case EPRDirection::Direction_Right:
		AddMovementInput(RightVector, MoveRight);
		break;
	default:
		break;
	}

	// 입력값에 따라 Gait를 설정합니다.
	if(bSprintInput == false)
	{
		// WalkToggleInput이 활성화 되었으면 입력값이 무엇이는 AllowedGait는 Walking으로 설정합니다.
		if(bWalkToggleInput == true)
		{
			GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Walking);
			return;
		}
		
		// 원의 방정식을 사용합니다.
		// 원의 중심이 (a,b)이고 반지름의 길이가 r인 원의 방정식
		// (x - a)^2 + (y - b)^2 = r^2
		
		float NewMoveForward = UKismetMathLibrary::Square(UKismetMathLibrary::Abs(MoveForward));
		float NewMoveRight = UKismetMathLibrary::Square(UKismetMathLibrary::Abs(MoveRight));
		if(MinRunInputAxis > UKismetMathLibrary::Sqrt(NewMoveForward + NewMoveRight))
		{
			GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Walking);
		}
		else
		{
			GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Running);
		}
	}
}

EPRDirection APRPlayerCharacter::GetMoveInputDirection() const
{
	const float NewInputMoveForward = GetMoveForward();
	const float NewInputMoveRight = GetMoveRight();
	
	if(NewInputMoveForward > 0.0f)
	{
		if(NewInputMoveRight < 0.0f)
		{
			return EPRDirection::Direction_ForwardLeft;
		}

		if(NewInputMoveRight == 0.0f)
		{
			return EPRDirection::Direction_Forward;
		}
		
		if(NewInputMoveRight > 0.0f)
		{
			return EPRDirection::Direction_ForwardRight;
		}
	}

	if(NewInputMoveForward == 0.0f)
	{
		if(NewInputMoveRight < 0.0f)
		{
			return EPRDirection::Direction_Left;
		}
		
		if(NewInputMoveRight > 0.0f)
		{
			return EPRDirection::Direction_Right;
		}
	}

	if(NewInputMoveForward < 0.0f)
	{
		if(NewInputMoveRight < 0.0f)
		{
			return EPRDirection::Direction_BackwardLeft;
		}

		if(NewInputMoveRight == 0.0f)
		{
			return EPRDirection::Direction_Backward;
		}
		
		if(NewInputMoveRight > 0.0f)
		{
			return EPRDirection::Direction_BackwardRight;
		}
	}

	return EPRDirection::Direction_None;
}

void APRPlayerCharacter::Sprint(bool bNewSprintInput)
{
	bWalkToggleInput = false;
	bSprintInput = bNewSprintInput;
	if(bSprintInput)
	{
		GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Sprinting);
	}
}

void APRPlayerCharacter::Walk()
{
	if(bWalkToggleInput == true)
	{
		bWalkToggleInput = false;
	}
	else
	{
		bWalkToggleInput = true;
	}
}
#pragma endregion

#pragma region TimeStopSystem
void APRPlayerCharacter::ActivateTimeStop(float TimeStopDuration, EPRCameraPostProcessMaterial NewMonochromeMode)
{
	ActivateMonochromeMode(NewMonochromeMode, true, false);
	// GetEffectSystem()->SetActivateTimeStop(true);
	// GetObjectPoolSystem()->SetActivateTimeStop(true);
	// GetTimeStopSystem()->ActivateTimeStop(TimeStopDuration);
	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsValid(PRGameMode) == true)
	{
		PRGameMode->ActivateTimeStop(TimeStopDuration);
	}
}

void APRPlayerCharacter::DeactivateTimeStop()
{
	OnDeactivateTimeStop();
	// GetTimeStopSystem()->DeactivateTimeStop();
	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsValid(PRGameMode) == true)
	{
		PRGameMode->DeactivateTimeStop();
	}
}

void APRPlayerCharacter::OnDeactivateTimeStop()
{
	DeactivateMonochromeMode(EPRCameraPostProcessMaterial::CameraPostProcessMaterial_TimeStop);
	DeactivateMonochromeMode(EPRCameraPostProcessMaterial::CameraPostProcessMaterial_BlackAndWhite);
	// GetEffectSystem()->SetActivateTimeStop(false);
	// GetObjectPoolSystem()->SetActivateTimeStop(false);
}

bool APRPlayerCharacter::IsActivateTimeStop() const
{
	// return GetTimeStopSystem()->IsActivateTimeStop();
	AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsValid(PRGameMode) == true)
	{
		return PRGameMode->IsActivateTimeStop();
	}
	
	return false; 
}
#pragma endregion

#pragma region Interact
void APRPlayerCharacter::AddToInteractableObjects(AActor* NewInteractableObject)
{
	// PRInteractInteractInterface를 상속하고 있는지 확인합니다.
	if(NewInteractableObject->GetClass()->ImplementsInterface(UPRInteractInterface::StaticClass()) == true)
	{
		InteractableObjects.AddUnique(NewInteractableObject);

		// InteractWidget에서 NewInteractableObject에 해당하는 상호작용 위젯을 추가합니다.
		APRPlayerController* PRPlayerController = Cast<APRPlayerController>(GetController());
		if(PRPlayerController != nullptr)
		{
			PRPlayerController->GetInGameHUD()->GetInteractWidget()->AddToInteractList(NewInteractableObject);
		}
	}
}

void APRPlayerCharacter::RemoveToInteractableObjects(AActor* NewInteractableObject)
{
	InteractableObjects.Remove(NewInteractableObject);

	if(SelectInteractIndex - 1 < 0)
	{
		SelectInteractIndex = 0;
	}
	else
	{
		SelectInteractIndex--;
	}

	// InteractWidget에서 NewInteractableObject에 해당하는 상호작용 위젯을 제거합니다.
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(GetController());
	if(PRPlayerController != nullptr)
	{
		PRPlayerController->GetInGameHUD()->GetInteractWidget()->SetSelectInteractIndex(SelectInteractIndex);
		PRPlayerController->GetInGameHUD()->GetInteractWidget()->RemoveToInteractList(NewInteractableObject);
	}
}

void APRPlayerCharacter::SelectInteraction(float Value)
{
	if(InteractableObjects.Num() == 0)
	{
		SelectInteractIndex = 0;
		return;
	}

	if(bResetInteractInput)
	{
		// 목록의 아래로 선택
		if(Value < 0.0f)
		{
			if(SelectInteractIndex + 1 == InteractableObjects.Num())
			{
				SelectInteractIndex = 0.0f;
			}
			else
			{
				SelectInteractIndex++;
			}
		}
		// 목록의 위로 선택
		else if(Value > 0.0f)
		{
			if(SelectInteractIndex - 1 < 0)
			{
				SelectInteractIndex = InteractableObjects.Num() - 1;
			}
			else
			{
				SelectInteractIndex--;
			}
		}

		bResetInteractInput = false;
	}

	// 중복을 방지하기 위한 변수 설정
	if(Value == 0.0f)
	{
		bResetInteractInput = true;
	}

	// 선택한 Slot에 하이라이트를 적용합니다.
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(GetController());
	if(PRPlayerController != nullptr)
	{
		PRPlayerController->GetInGameHUD()->GetInteractWidget()->SetSelectInteractIndex(SelectInteractIndex);
	}
}

void APRPlayerCharacter::ExecuteInteract()
{
	if(InteractableObjects.IsValidIndex(SelectInteractIndex) == true && IsValid(InteractableObjects[SelectInteractIndex]) == true)
	{
		if(InteractableObjects[SelectInteractIndex]->GetClass()->ImplementsInterface(UPRInteractInterface::StaticClass()) == true)
		{
			IPRInteractInterface::Execute_OnInteract(InteractableObjects[SelectInteractIndex]);
		}
	}
}
#pragma endregion

#pragma region TargetingSystem
void APRPlayerCharacter::ActivateLockOnTarget()
{
	if(GetTargetingSystem()->IsActivateLockOnTarget() == false)
	{
		GetTargetingSystem()->ActivateLockOnTarget();
	}
	else
	{
		GetTargetingSystem()->CancelLockOnTarget();
	}
}

void APRPlayerCharacter::ReadyToChangeTarget(bool bIsReady)
{
	bReadyToChangeTarget = bIsReady;
	TargetingSystem->SetActivateDynamicCameraLock(bIsReady);
}
#pragma endregion

#pragma region EffectSystem

#pragma endregion 

#pragma region Dodge
bool APRPlayerCharacter::IsActivateExtremeDodgeArea()
{
	if(ExtremeDodgeArea->GetCollisionEnabled() == ECollisionEnabled::QueryOnly)
	{
		return true;
	}

	return false;
}

void APRPlayerCharacter::ActivateExtremeDodgeArea()
{
	ExtremeDodgeArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APRPlayerCharacter::DeactivateExtremeDodgeArea()
{
	ExtremeDodgeArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APRPlayerCharacter::Dodge()
{
	if(GetStateSystem()->IsActionable(EPRAction::Action_Dodge) == true
		&& DodgePRAnimMontages.Num() != 0)
	{
		// 일반 공격 Index를 초기화합니다.
		InitializePlayNormalAttackIndex();
		
		// 현재 사용하는 무기가 발도 상태면 무기를 납도합니다.
		if(IsValid(GetWeaponSystem()->GetEquippedWeapon()) == true && GetWeaponSystem()->GetEquippedWeapon()->IsDraw() == true)
		{
			GetWeaponSystem()->GetEquippedWeapon()->Sheath();
		}
		
		GetStateSystem()->SetCanCancelAction(false);
		GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
		GetStateSystem()->SetActionable(EPRAction::Action_Dodge, false);
		GetStateSystem()->SetIsInvincible(true);

		// 동작을 캔슬할 경우 잔상효과를 활성화합니다.
		if(GetAnimSystem()->IsPlayPRAnimMontage() == true)
		{
			// GetAfterImage()->Activate(GetMesh(), GetMesh()->GetComponentTransform());
			ActivateAfterImage();
		}

		if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == false)
		{
			if(IsMoveInput() == true)
			{
				RotationInputDirection();
				GetAnimSystem()->PlayPRAnimMontage(*DodgePRAnimMontages.Find(*FString::FromInt(ForwardDodgePRAnimMontageID)), GetActorForwardVector());
			}
			else
			{
				GetAnimSystem()->PlayPRAnimMontage(*DodgePRAnimMontages.Find(*FString::FromInt(BackwardDodgePRAnimMontageID)), GetActorForwardVector() * -1.0f);
			}
		}
		else
		{
			RotationInputDirection();
			GetMovementSystem()->ActivateAerial(true);
			GetAnimSystem()->PlayPRAnimMontage(*DodgePRAnimMontages.Find(*FString::FromInt(AerialForwardDodgePRAnimMontageID)), GetActorForwardVector());
		}
	}
}
#pragma endregion

#pragma region Dodge/Sprint
void APRPlayerCharacter::DodgeAndSprint(bool bNewSprintInput)
{
	bWalkToggleInput = false;
	bSprintInput = bNewSprintInput;
	// if(bSprintInput)
	// {
	// 	if(GetWorld()->GetTimerManager().IsTimerActive(SprintTimerHandle) == false && GetMovementSystem()->IsEqualAllowedGait(EPRGait::Gait_Sprinting) == false)
	// 	{
	// 		GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, FTimerDelegate::CreateLambda([&]()
	// 		{
	// 			GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Sprinting);
	// 			
	// 			GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
	// 		}), SprintableHoldTime, false);
	// 	}
	// }
	// else
	// {
	// 	if(GetMovementSystem()->IsEqualAllowedGait(EPRGait::Gait_Sprinting) == false)
	// 	{
	// 		Dodge();
	// 	}
	// 	
	// 	if(GetWorld()->GetTimerManager().IsTimerActive(SprintTimerHandle) == true)
	// 	{
	// 		GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
	// 	}	
	// }

	if(bSprintInput)
	{
		if(!bDodgeInput)
		{
			bDodgeInput = true;
			Dodge();
		}
		
		if(GetWorld()->GetTimerManager().IsTimerActive(SprintTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Sprinting);
				
				GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
			}), SprintableHoldTime, false);	
		}
	}
	else
	{
		bDodgeInput = false;
		
		if(GetWorld()->GetTimerManager().IsTimerActive(SprintTimerHandle) == true)
		{
			GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
		}
	}
}
#pragma endregion 

#pragma region CommandInput
void APRPlayerCharacter::NormalAttack()
{
	// 캐릭터가 사망한 상태에서 실행하지 않습니다.
	if(IsDead() == true)
	{
		return;
	}
}

void APRPlayerCharacter::ChargedAttack(FKey Key)
{
	// 캐릭터가 사망한 상태에서 실행하지 않습니다.
	if(IsDead() == true)
	{
		return;
	}
}
#pragma endregion 

#pragma region NormalAttack
void APRPlayerCharacter::InitializePlayNormalAttackIndex()
{
}

void APRPlayerCharacter::IncreasePlayNormalAttackIndex()
{
}
#pragma endregion 

#pragma region Effect
void APRPlayerCharacter::ActivateAfterImage()
{
	GetObjectPoolSystem()->ActivatePooledObject(AfterImage);
}
#pragma endregion 

#pragma region SkillPalette
void APRPlayerCharacter::ActivateBattleSkill(EPRCommandSkill NewPRCommandSkill)
{
	if(IsValid(GetSkillSystem()->GetSkillFromCommand(NewPRCommandSkill)) == true)
	{
		APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if(IsValid(PRPlayerController) == true)
		{
			if(PRPlayerController->IsGamepadInput() == true)
			{
				if(bIsOpenSkillPalette)
				{
					GetSkillSystem()->GetSkillFromCommand(NewPRCommandSkill)->ActivateSkill();
				}
			}
			else
			{
				GetSkillSystem()->GetSkillFromCommand(NewPRCommandSkill)->ActivateSkill();
			}
		}
	} 
}

void APRPlayerCharacter::OpenSkillPalette(bool bNewIsOpen)
{
	bIsOpenSkillPalette = bNewIsOpen;
	if(OnOpenSkillPaletteDelegate.IsBound() == true)
	{
		OnOpenSkillPaletteDelegate.Broadcast(bNewIsOpen);
	}
}
#pragma endregion 

#pragma region ComboCount
void APRPlayerCharacter::InitializeComboCount()
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PRPlayerController != nullptr)
	{
		if(PRPlayerController->GetInGameHUD()->GetComboCountWidget()->GetVisibility() == ESlateVisibility::Visible)
		{
			PRPlayerController->GetInGameHUD()->GetComboCountWidget()->SetVisibility(ESlateVisibility::Collapsed);
		}

		bActivateComboCount = false;
		ComboCountResetRemaining = ComboCountResetTime;
		ComboCountResetElapsed = 0.0f;
		ComboCount = 0;
		
		// GetWorld()->GetTimerManager().ClearTimer(ComboCountTimerHandle);
	}
}

void APRPlayerCharacter::ActivateComboCount()
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PRPlayerController != nullptr)
	{
		if(PRPlayerController->GetInGameHUD()->GetComboCountWidget()->GetVisibility() == ESlateVisibility::Collapsed)
		{
			PRPlayerController->GetInGameHUD()->GetComboCountWidget()->SetVisibility(ESlateVisibility::Visible);
		}

		bActivateComboCount = true;
		ComboCount++;
		// ComboCount가 최신화 되었으므로 ComboCount의 초기화까지의 남은 시간과 ComboCount의 초기화 시간이 실행된 경과시간을 초기화합니다.
		ComboCountResetRemaining = ComboCountResetTime;
		ComboCountResetElapsed = 0.0f;

		// ComboCount가 최신화 되었으므로 ComboCountTimerHandle 타이머가 작동 중이면 타이머를 해제합니다.
		// if(GetWorld()->GetTimerManager().IsTimerActive(ComboCountTimerHandle) == true)
		// {
		// 	GetWorld()->GetTimerManager().ClearTimer(ComboCountTimerHandle);
		// }
		//
		// // ComboCount를 초기화하는 타이머를 실행합니다.
		// GetWorld()->GetTimerManager().SetTimer(ComboCountTimerHandle, this, &APRPlayerCharacter::InitializeComboCount, ComboCountResetTime, false);
	}

	if(OnComboCountChangedDelegate.IsBound() == true)
	{
		OnComboCountChangedDelegate.Broadcast(ComboCount);
	}
}

void APRPlayerCharacter::UpdateComboCount(float DeltaTime)
{
	if(bActivateComboCount)
	{
		ComboCountResetElapsed += DeltaTime;
		ComboCountResetRemaining = ComboCountResetTime - ComboCountResetElapsed;

		// ComboCount 초기화 시간이 되었을 때
		if(ComboCountResetElapsed >= ComboCountResetTime)
		{
			InitializeComboCount();
		}
	}
}

float APRPlayerCharacter::GetComboCountResetTimeRatio() const
{
	// if(GetWorld()->GetTimerManager().IsTimerActive(ComboCountTimerHandle) == false)
	// {
	// 	return 0.0f;
	// }
	//
	// return FMath::Clamp(GetWorld()->GetTimerManager().GetTimerRemaining(ComboCountTimerHandle) / ComboCountResetTime, 0.0f, 1.0f);

	if(bActivateComboCount == false)
	{
		return 0.0f;
	}

	return FMath::Clamp(ComboCountResetRemaining / ComboCountResetTime, 0.0f, 1.0f);
}
#pragma endregion 

#pragma region InGameHUD
void APRPlayerCharacter::InitializeInGameHUD()
{
	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(IsValid(PRPlayerController) == true)
	{
		PRPlayerController->GetInGameHUD()->InitializeWidgets();
	}
}
#pragma endregion 

