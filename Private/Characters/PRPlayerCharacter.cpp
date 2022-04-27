// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRPlayerCharacter.h"
#include "Enums/Enum_PRDirectionVector.h"
#include "Components/PRDodgeSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Weapons/PRBaseWaepon.h"
#include "Weapons/PROverlapCollision.h"
#include "Effects/PRGhostTrail.h"
#include "Enums/Enum_PRDirection.h"
#include "Enums/Enum_PRWeaponEquipPosition.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APRPlayerCharacter::APRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerCharacter"));
	
	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	BaseSpringArmLength = 400.0f;
	SpringArm->TargetArmLength = BaseSpringArmLength;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 200.0f);
	SpringArm->bUsePawnControlRotation = true;										// 컨트롤러를 기준으로 SpringArm을 회전합니다.
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.0f;

	// FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;									// 카메라가 SpringArm을 기준으로 회전하지 않습니다.
	FollowCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	// Camera
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	CameraZoomOutCurveFloat = nullptr;

	// CharacterMovement
	DoubleJumpAnimMontage = nullptr;
	DoubleJumpNiagaraEffect = nullptr;
	DoubleJumpNiagaraEffectColor = FLinearColor(20.000017f, 14.999998, 200.0, 1.0f);
	DoubleJumpNiagaraEffectSpawnSocketName = FName("ball_r");
	bIsDoubleJump = false;
	bCanDoubleJump = true;
	DoubleJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	// TargetingSystem
	TargetingSystem = CreateDefaultSubobject<UPRTargetingSystemComponent>(TEXT("TargetingSystem"));

	// Effects
	GhostTrail = nullptr;
	GhostTrailSpawnCount = 3;
	GhostTrailObjectPool.Empty();

	// AttackOverlapCollision
	bDrawDebugAttackOverlapCollision = false;
	AttackOverlapCollision = nullptr;
	AttackOverlapCollisionSpawnCount = 2;
	AttackOverlapCollisionObjectPool.Empty();
}

void APRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeCameraZoomOutTimeline();
	InitializeGhostTrailObjectPool(GhostTrailSpawnCount);
	InitializeAttackOverlapCollision(AttackOverlapCollisionSpawnCount);
}

void APRPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CameraZoomOutTimeline.TickTimeline(DeltaSeconds);

	if(MovementSystem->GetGait() == EPRGait::Gait_Sprinting)
	{
		CameraZoomOutWhenSprint(true);
	}
	else
	{
		CameraZoomOutWhenSprint(false);
	}
}

void APRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APRPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APRPlayerCharacter::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &APRPlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APRPlayerCharacter::LookUpRate);

	PlayerInputComponent->BindAxis("MoveForward", this, &APRPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APRPlayerCharacter::MoveRight);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APRPlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction<TDelegate<void(EPRGait)>>("Sprint", IE_Pressed, this, &APRPlayerCharacter::SetRunOrSprint, EPRGait::Gait_Sprinting);
	PlayerInputComponent->BindAction<TDelegate<void(EPRGait)>>("Sprint", IE_Released, this, &APRPlayerCharacter::SetRunOrSprint, EPRGait::Gait_Running);
	PlayerInputComponent->BindAction("ToggleWalkRun", IE_Pressed, this, &APRPlayerCharacter::ToggleWalkRun);

	PlayerInputComponent->BindAction("TargetLockOn", IE_Pressed, this, &APRPlayerCharacter::ExecuteLockOnTarget);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APRPlayerCharacter::Dodge);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APRPlayerCharacter::Attack);
	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &APRPlayerCharacter::SwapWeapon);
}

void APRPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsDoubleJump = false;
	bCanDoubleJump = true;
	MovementSystem->SetWasAerial(false);
}

#pragma region Camera
void APRPlayerCharacter::Turn(float Value)
{
	if(TargetingSystem->IsLockOnTarget() == true && Value != 0.0f)
	{
		TargetingSystem->ChangeLockOnTargetForTurnValue(EPRInputMode::InputMode_Mouse, Value);
	}
	else
	{
		AddControllerYawInput(Value);
	}
}

void APRPlayerCharacter::LookUp(float Value)
{
	if(TargetingSystem->IsLockOnTarget() == false)
	{
		AddControllerPitchInput(Value);
	}
}

void APRPlayerCharacter::TurnRate(float Rate)
{
	if(TargetingSystem->IsLockOnTarget() == true && Rate != 0.0f)
	{
		TargetingSystem->ChangeLockOnTargetForTurnValue(EPRInputMode::InputMode_Gamepad, Rate);		
	}
	else
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APRPlayerCharacter::LookUpRate(float Rate)
{
	if(TargetingSystem->IsLockOnTarget() == false)
	{
		AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APRPlayerCharacter::InitializeCameraZoomOutTimeline()
{
	if(CameraZoomOutCurveFloat)
	{
		// Curve가 사용할 Callback 함수입니다.
		FOnTimelineFloat TimelineProgress;

		// Callback 함수에서 사용할 함수를 바인딩합니다.
		TimelineProgress.BindUFunction(this, FName("CameraZoomOut"));

		// Timeline에 Curve를 추가합니다.
		CameraZoomOutTimeline.AddInterpFloat(CameraZoomOutCurveFloat, TimelineProgress, NAME_None, TEXT("CameraZoomOut"));
	}
}

void APRPlayerCharacter::CameraZoomOut(float Value)
{
	SpringArm->TargetArmLength = Value;
}

void APRPlayerCharacter::CameraZoomOutWhenSprint(bool bFlag)
{
	if(bFlag)
	{
		CameraZoomOutTimeline.Play();
	}
	else
	{
		CameraZoomOutTimeline.Reverse();
	}
}
#pragma endregion 

#pragma region CharacterMovement
void APRPlayerCharacter::RotateInputDirection(bool bIsReverse)
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
	// 입력이 있을 경우 컨트롤러를 기준으로 회전합니다.
	else
	{
		InputDirectionYaw = InputRotator.Yaw + GetControlRotation().Yaw;
	}

	const FRotator InputDirectionRotator = FRotator(0.0f, InputDirectionYaw, 0.0f);
	SetActorRotation(InputDirectionRotator);
}

float APRPlayerCharacter::GetMoveForward() const
{
	return InputComponent->GetAxisValue("MoveForward");
}

float APRPlayerCharacter::GetMoveRight() const
{
	return InputComponent->GetAxisValue("MoveRight");
}

TTuple<float, float> APRPlayerCharacter::FixDiagonalGamepadValues(float NewForwardAxis, float NewRightAxis) const
{
	// 미세한 값을 1.0이나 -1.0으로 보정합니다.
	const float ForwardAxis = NewForwardAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.0f),
																			FVector2D(1.0f, 1.2f), FMath::Abs(NewRightAxis));
	const float RightAxis = NewRightAxis * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.0f),
																		FVector2D(1.0f, 1.2f), FMath::Abs(NewForwardAxis));

	const float FixForwardAxis = FMath::Clamp(ForwardAxis, -1.0f, 1.0f);
	const float FixRightAxis = FMath::Clamp(RightAxis, -1.0f, 1.0f);

	return MakeTuple(FixForwardAxis, FixRightAxis);
}

TTuple<FVector, FVector> APRPlayerCharacter::GetControlForwardRightVector() const
{
	const FRotator NewControlRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(NewControlRotation);
	const FVector RightVector = UKismetMathLibrary::GetRightVector(NewControlRotation);
	
	return MakeTuple(ForwardVector, RightVector);
}

void APRPlayerCharacter::AddPlayerMovementInput(EPRDirectionVector PRDirectionVector)
{
	if(MovementSystem->IsEqualMovementState(EPRMovementState::MovementState_Ground) == true
		|| MovementSystem->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
	{
		TTuple<FVector, FVector> ForwardRightVector = GetControlForwardRightVector();
		const FVector ForwardVector = ForwardRightVector.Get<0>();
		const FVector RightVector = ForwardRightVector.Get<1>();

		TTuple<float, float> GamepadValues = FixDiagonalGamepadValues(GetMoveForward(), GetMoveRight());
		const float MoveForward = GamepadValues.Get<0>();
		const float MoveRight = GamepadValues.Get<1>();

		switch(PRDirectionVector)
		{
		case EPRDirectionVector::DirectionVector_ForwardVector:
			AddMovementInput(ForwardVector, MoveForward);
			break;
		case EPRDirectionVector::DirectionVector_RightVector:
			AddMovementInput(RightVector, MoveRight);
			break;
		default:
			break;
		}
	}
}

EPRDirection APRPlayerCharacter::GetMoveInputDirection() const
{
	const float MoveForward = InputComponent->GetAxisValue("MoveForward");
	const float MoveRight = InputComponent->GetAxisValue("MoveRight");
	
	if(MoveForward >= 0.0f)
	{
		if(MoveRight == 0.0f)
		{
			return EPRDirection::Direction_Forward;
		}

		if(MoveForward == 0.0f)
		{
			if(MoveRight > 0.0f)
			{
				return EPRDirection::Direction_Right;
			}

			return EPRDirection::Direction_Left;
		}

		if(MoveRight > 0.0f)
		{
			return EPRDirection::Direction_ForwardRight;
		}

		return EPRDirection::Direction_ForwardLeft;
	}

	if(MoveRight == 0.0f)
	{
		return EPRDirection::Direction_Backward;
	}

	if(MoveRight > 0.0f)
	{
		return EPRDirection::Direction_BackwardRight;
	}

	return EPRDirection::Direction_BackwardLeft;
}

void APRPlayerCharacter::ToggleWalkRun()
{
	switch(MovementSystem->GetAllowedGait())
	{
	case EPRGait::Gait_Walking:
		MovementSystem->SetAllowedGait(EPRGait::Gait_Running);
		break;
	case EPRGait::Gait_Running:
		MovementSystem->SetAllowedGait(EPRGait::Gait_Walking);
		break;
	default:
		break;			
	}
}

void APRPlayerCharacter::Jump()
{
	if(bIsDoubleJump)
	{
		return;
	}

	if(WeaponSystem->IsDrawWeapons() == true)
	{
		WeaponSystem->SheathWeapons(true);
	}

	if(MovementSystem->IsEqualMovementState(EPRMovementState::MovementState_InAir) == false)
	{
		ACharacter::Jump();
	}
	else if(bCanDoubleJump)
	{
		DoubleJump();
	}
}

void APRPlayerCharacter::DoubleJump()
{
	bIsDoubleJump = true;
	bCanDoubleJump = false;
	StateSystem->SetCanMove(true);
	StateSystem->SetOnDodge(false);
	
	const float CurrentSpeed = MovementSystem->GetSpeed() * 0.8f;
	const float MoveForward = GetMoveForward();
	const float MoveRight = GetMoveRight();
	TTuple<FVector, FVector> ForwardRightVector = GetControlForwardRightVector();

	MovementSystem->ExecuteAerial(true);
	RotateInputDirection();
	PlayAnimMontage(DoubleJumpAnimMontage);
	ExecuteDoubleJumpNiagaraEffect();
	
	const FVector NewForwardVector = ForwardRightVector.Get<0>() * MoveForward * CurrentSpeed;
	const FVector NewRightVector = ForwardRightVector.Get<1>() * MoveRight * CurrentSpeed;

	const FVector DirectionVector = NewForwardVector + NewRightVector + FVector(0.0f, 0.0f, DoubleJumpVelocity);
	LaunchCharacter(DirectionVector, false, false);
	MovementSystem->ExecuteAerial(false);
}

void APRPlayerCharacter::ExecuteDoubleJumpNiagaraEffect()
{
	if(DoubleJumpNiagaraEffect)
	{
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DoubleJumpNiagaraEffect,
																									GetMesh()->GetSocketLocation(DoubleJumpNiagaraEffectSpawnSocketName));
		SpawnNiagaraComponent->SetVariableLinearColor("EffectLinearColor", DoubleJumpNiagaraEffectColor);
	}
}

bool APRPlayerCharacter::IsDoubleJump() const
{
	return bIsDoubleJump;
}

void APRPlayerCharacter::MoveForward(float Value)
{
	if(Controller != nullptr && StateSystem->IsCanMove() == true)
	{
		if(FMath::Abs(Value) != 0.0f)
		{
			if(!bIsDoubleJump)
			{
				AnimSystem->StopPRAnimMontage();
				StateSystem->SetOnAttack(false);
				StateSystem->SetOnDodge(false);
			}

			if(WeaponSystem->IsDrawWeapons() == true)
			{
				WeaponSystem->SheathWeapons(true);
			}
		}
		
		AddPlayerMovementInput(EPRDirectionVector::DirectionVector_ForwardVector);
	}
}

void APRPlayerCharacter::MoveRight(float Value)
{
	if(Controller != nullptr && StateSystem->IsCanMove() == true)
	{
		if(FMath::Abs(Value) != 0.0f)
		{
			if(!bIsDoubleJump)
			{
				AnimSystem->StopPRAnimMontage();
				StateSystem->SetOnAttack(false);
				StateSystem->SetOnDodge(false);
			}

			if(WeaponSystem->IsDrawWeapons() == true)
			{
				WeaponSystem->SheathWeapons(true);
			}
		}
		
		AddPlayerMovementInput(EPRDirectionVector::DirectionVector_RightVector);
	}
}
#pragma endregion

#pragma region WeaponSystem
void APRPlayerCharacter::SwapWeapon()
{
	WeaponSystem->SwapWeapons();
}
#pragma endregion 

#pragma region TargetingSystem
void APRPlayerCharacter::ExecuteLockOnTarget()
{
	if(TargetingSystem->IsLockOnTarget() == false)
	{
		TargetingSystem->ExecuteLockOnTarget();
	}
	else
	{
		TargetingSystem->CancelLockOnTarget();
	}
}
#pragma endregion

#pragma region Effect
APRGhostTrail* APRPlayerCharacter::GetGhostTrail()
{
	int32 Index = 0;
	for(APRGhostTrail* PoolingGhostTrail : GhostTrailObjectPool)
	{
		if(PoolingGhostTrail->IsHidden() == true)
		{
			return PoolingGhostTrail;
		}

		Index++;

		if(GhostTrailObjectPool.Num() == Index)
		{
			APRGhostTrail* NewGhostTrail = SpawnGhostTrail();
			if(IsValid(NewGhostTrail) == true)
			{
				GhostTrailObjectPool.Add(NewGhostTrail);
				return NewGhostTrail;
			}

			return nullptr;
		}
	}

	return nullptr;
}

// void APRPlayerCharacter::DisableGhostTrail(APRGhostTrail* EnableGhostTrail)
// {
// 	EnableGhostTrail->SetActorHiddenInGame(true);
// }

APRGhostTrail* APRPlayerCharacter::SpawnGhostTrail() const
{
	if(GhostTrail)
	{
		APRGhostTrail* NewGhostTrail = GetWorld()->SpawnActor<APRGhostTrail>(GhostTrail, GetMesh()->GetComponentTransform());
		NewGhostTrail->SetActorHiddenInGame(true);
		// NewGhostTrail->OnOpacityIsZeroDelegate.AddUFunction(this, FName("DisableGhostTrail"));

		return NewGhostTrail;
	}

	return nullptr;
}

void APRPlayerCharacter::InitializeGhostTrailObjectPool(int32 SpawnCount)
{
	if(GhostTrail)
	{
		for(int32 Index = 0; Index < SpawnCount; Index++)
		{
			GhostTrailObjectPool.Add(SpawnGhostTrail());
		}
	}
}
#pragma endregion

#pragma region Dodge
void APRPlayerCharacter::Dodge()
{
	if(StateSystem->IsCanDodge() == true && StateSystem->IsOnDodge() == false)
	{
		GetGhostTrail()->Execute(this, GetMesh()->GetComponentTransform());

		StateSystem->SetCanMove(false);
		StateSystem->SetOnDodge(true);
		
		DodgeSystem->ExecuteDodge(GetMoveInputDirection());
	}
}
#pragma endregion

#pragma region Attack
void APRPlayerCharacter::Attack()
{
	if(StateSystem->IsCanAttack() == true && GetCharacterMovement()->IsFalling() == false)
	{
		StateSystem->SetCanMove(false);
		StateSystem->SetOnAttack(true);
		StateSystem->SetOnDodge(false);
		WeaponSystem->SetCanSwapWeapon(false);
		
		if(WeaponSystem->IsDrawWeapons() == false)
		{
			WeaponSystem->DrawWeapons();
		}

		if(TargetingSystem->IsLockOnTarget() == true)
		{
			const float LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetingSystem->GetTarget()->GetActorLocation()).Yaw;
			SetActorRotation(FRotator(0.0f, LookAtRotation, 0.0f));
		}
		else
		{
			RotateInputDirection();
		}
		
		AnimSystem->PlayPRAnimMontage(WeaponSystem->GetEquipWeapon(EPRWeaponEquipPosition::WeaponEquipPosition_Main)->GetWeaponPRAnimMontage());
		WeaponSystem->GetEquipWeapon(EPRWeaponEquipPosition::WeaponEquipPosition_Main)->IncreaseWeaponPRAnimMontageIndex();
	}
	
}
#pragma endregion 

#pragma region AttackOverlapCollision
APROverlapCollision* APRPlayerCharacter::GetAttackOverlapCollision()
{
	int32 Index = 0;
	for(APROverlapCollision* PoolingAttackOverlapCollision : AttackOverlapCollisionObjectPool)
	{
		if(PoolingAttackOverlapCollision->IsExecuteOverlap() == false)
		{
			return PoolingAttackOverlapCollision;
		}

		Index++;

		if(AttackOverlapCollisionObjectPool.Num() == Index)
		{
			APROverlapCollision* NewAttackOverlapCollision = SpawnAttackOverlapCollision();
			if(IsValid(NewAttackOverlapCollision) == true)
			{
				AttackOverlapCollisionObjectPool.Add(NewAttackOverlapCollision);
				return NewAttackOverlapCollision;
			}

			return nullptr;
		}
	}

	return nullptr;
}

APROverlapCollision* APRPlayerCharacter::SpawnAttackOverlapCollision()
{
	if(AttackOverlapCollision)
	{
		APROverlapCollision* NewAttackOverlapCollision = GetWorld()->SpawnActor<APROverlapCollision>(AttackOverlapCollision, GetMesh()->GetComponentTransform());
		NewAttackOverlapCollision->InitializePROwner(this);
		NewAttackOverlapCollision->DrawDebugOverlapCollision(bDrawDebugAttackOverlapCollision);
	
		return NewAttackOverlapCollision;
	}

	return nullptr;
}

void APRPlayerCharacter::InitializeAttackOverlapCollision(int32 SpawnCount)
{
	if(AttackOverlapCollision)
	{
		for(int32 Index = 0; Index < SpawnCount; Index++)
		{
			AttackOverlapCollisionObjectPool.Add(SpawnAttackOverlapCollision());
		}
	}
}
#pragma endregion 
