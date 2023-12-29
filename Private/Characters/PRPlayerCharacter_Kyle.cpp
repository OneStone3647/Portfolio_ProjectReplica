// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRPlayerCharacter_Kyle.h"
#include "NiagaraComponent.h"
#include "PRPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Effect/PRNiagaraEffect.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Skills/Characters/Kyle/PRSkill_Kyle_DodgeAttack.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Objects/PRScreenShatter.h"
#include "Blueprint/UserWidget.h"

APRPlayerCharacter_Kyle::APRPlayerCharacter_Kyle()
{
	// SceneCapture
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(FollowCamera);
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	
	// Dodge
	ForwardDodgePRAnimMontageID = 1000;
	BackwardDodgePRAnimMontageID = 1001;
	LeftDodgePRAnimMontageID = 1002;
	RightDodgePRAnimMontageID = 1003;
	AerialForwardDodgePRAnimMontageID = 1004;
	
	// CharacterInfo
	AttackMode = EPRAttackMode::AttackMode_General;
	
	// NormalAttack
	GeneralNormalAttackPRAnimMontageFromID = 1000;
	GeneralNormalAttackPRAnimMontageToID = 1005;
	AwakeningNormalAttackPRAnimMontageFromID = 1100;
	AwakeningNormalAttackPRAnimMontageToID = 1106;
	GeneralNormalPRComboAnimMontage = FPRComboAnimMontage();
	AwakeningNormalPRComboAnimMontage = FPRComboAnimMontage();

	// ChargedAttack
	// ChargedAttackPRAnimMontageDataTable = nullptr;
	// ChargedNormalAttackPRAnimMontageFromID = 1201;
	// ChargedNormalAttackPRAnimMontageToID = 1201;
	// ChargedAttackPRAnimMontages.Empty();
	// bExecuteChargedAttack = false;
	// ChargedAttackableHoldTime = 0.15f;
	// ChargedAttackInputKey = FKey();
	// ChargedAttackPRAnimMontage = FPRAnimMontage();
}

void APRPlayerCharacter_Kyle::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APRPlayerCharacter_Kyle::BeginPlay()
{
	Super::BeginPlay();
	
	// AnimSystem
	InitializePRAnimMontages();
}

void APRPlayerCharacter_Kyle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APRPlayerCharacter_Kyle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// CommandInput
	PlayerInputComponent->BindAction("NormalAttack", IE_Pressed, this, &APRPlayerCharacter_Kyle::NormalAttack);
}

#pragma region Camera
void APRPlayerCharacter_Kyle::ActivateScreenShatter_Implementation()
{
	// 블루프린트에서 구현합니다.
	// C++ 코드로 작성하면 Destructible가 작동하지 않습니다.

	
	// if(IsValid(SceneCapture))
	// {
	// 	SceneCapture->CaptureScene();
	// 	APRScreenShatter* NewScreenShatter = GetWorld()->SpawnActor<APRScreenShatter>(ScreenShatter);
	// 	if(IsValid(NewScreenShatter))
	// 	{
	// 		// ScreenShatter 이외에 아무것도 보이지 않아야합니다.
	// 		NewScreenShatter->SetActorLocation(FVector(0.0f, 0.0f, -4000.0f));
	// 	
	// 		// ScreenShatterWidget을 생성하고 Viewport에 추가합니다.
	// 		APRPlayerController* PRPlayerController = Cast<APRPlayerController>(GetController());
	// 		if(PRPlayerController != nullptr)
	// 		{
	// 			UUserWidget* NewScreenShatterWidget = PRPlayerController->CreateWidgetFromClassReference(ScreenShatterWidget);
	// 			if(IsValid(NewScreenShatterWidget))
	// 			{
	// 				// Viewport의 제일 상단에 출력합니다.
	// 				NewScreenShatterWidget->AddToViewport(1);
	// 			}
	// 		}
	// 	}
	// }
}
#pragma endregion

#pragma region MovementInput
void APRPlayerCharacter_Kyle::Jump()
{
	Super::Jump();

	/** 점프시 회피공격을 종료합니다. */
	SetActivateDodgeAttack(false);
}

void APRPlayerCharacter_Kyle::StopJumping()
{
	Super::StopJumping();

	/** 점프종료시 회피공격을 종료합니다. */
	SetActivateDodgeAttack(false);
}

void APRPlayerCharacter_Kyle::AddPlayerMovementInput(EPRDirection Direction)
{
	Super::AddPlayerMovementInput(Direction);
}
#pragma endregion

#pragma region AnimSystem
void APRPlayerCharacter_Kyle::InitializePRAnimMontages()
{
	// NormalAttack AnimMontages
	GeneralNormalPRComboAnimMontage = FPRComboAnimMontage(GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToArray(GeneralNormalAttackPRAnimMontageFromID, GeneralNormalAttackPRAnimMontageToID));	
	AwakeningNormalPRComboAnimMontage = FPRComboAnimMontage(GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToArray(AwakeningNormalAttackPRAnimMontageFromID, AwakeningNormalAttackPRAnimMontageToID));	
}
#pragma endregion 

#pragma region SkillSystem
UPRBaseSkill* APRPlayerCharacter_Kyle::GetSkillFromCommand(EPRCommandSkill NewCommandSkill) const
{
	if(NewCommandSkill == EPRCommandSkill::CommandSkill_Ultimate)
	{
		switch(AttackMode)
		{
		case EPRAttackMode::AttackMode_General:
			return Super::GetSkillFromCommand(EPRCommandSkill::CommandSkill_GeneralUltimate);
		case EPRAttackMode::AttackMode_Awakening:
			return Super::GetSkillFromCommand(EPRCommandSkill::CommandSkill_AwakeningUltimate);
		default:
			return Super::GetSkillFromCommand(NewCommandSkill);
		}
	}
	
	return Super::GetSkillFromCommand(NewCommandSkill);
}
#pragma endregion 

#pragma region Dodge
void APRPlayerCharacter_Kyle::Dodge()
{
	Super::Dodge();

	/** 회피시 회피공격을 종료합니다. */
	SetActivateDodgeAttack(false);
}
#pragma endregion 

#pragma region CommandInput
void APRPlayerCharacter_Kyle::NormalAttack()
{
	Super::NormalAttack();
	
	GetStateSystem()->SetActionable(EPRAction::Action_Dodge, true);			// 공격 도중 공격을 캔슬하고 회피할 수 있도록 설정합니다.

	UPRSkill_Kyle_DodgeAttack* DodgeAttackSkill = Cast<UPRSkill_Kyle_DodgeAttack>(GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_DodgeAttack));
	if(IsValid(DodgeAttackSkill) == true
		&& (DodgeAttackSkill->IsActivateSkill() == true || GetStateSystem()->IsActionable(EPRAction::Action_DodgeAttack) == true))
	{
		GetWeaponSystem()->DrawWeapon(static_cast<int32>(EPRAttackMode::AttackMode_General));
		DodgeAttackSkill->ActivateSkill();
	}
	else
	{
		ExecuteNormalAttack();
	}
}

// void APRPlayerCharacter_Kyle::ChargedAttack(FKey Key)
// {
// 	Super::ChargedAttack(Key);
//
// 	// HoldInput
// 	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
// 	if(IsValid(PRPlayerController) == true)
// 	{
// 		if(PRPlayerController->OnHoldInputKeyDelegate.IsBound() == false)
// 		{
// 			PRPlayerController->InitializeHoldInputKey(Key, ChargedAttackableHoldTime);
// 			PRPlayerController->OnHoldInputKeyDelegate.AddDynamic(this, &APRPlayerCharacter_Kyle::ExecuteChargedAttack);
// 		}
//
// 		PRPlayerController->SetExecuteHoldInput(true);
// 	}
// 	
// 	bExecuteChargedAttack = false;
// }
#pragma endregion 

#pragma region NormalAttack
void APRPlayerCharacter_Kyle::InitializePlayNormalAttackIndex()
{
	if(GetWeaponSystem()->GetEquippedWeaponIndex() == static_cast<int32>(EPRAttackMode::AttackMode_General))
	{
		GeneralNormalPRComboAnimMontage.InitializePlayIndex();
	}
	else if(GetWeaponSystem()->GetEquippedWeaponIndex() == static_cast<int32>(EPRAttackMode::AttackMode_Awakening))
	{
		AwakeningNormalPRComboAnimMontage.InitializePlayIndex();
	}
}

void APRPlayerCharacter_Kyle::IncreasePlayNormalAttackIndex()
{
	if(GetWeaponSystem()->GetEquippedWeaponIndex() == static_cast<int32>(EPRAttackMode::AttackMode_General))
	{
		GeneralNormalPRComboAnimMontage.IncreasePlayIndex();
	}
	else if(GetWeaponSystem()->GetEquippedWeaponIndex() == static_cast<int32>(EPRAttackMode::AttackMode_Awakening))
	{
		AwakeningNormalPRComboAnimMontage.IncreasePlayIndex();
	}
}

void APRPlayerCharacter_Kyle::ExecuteNormalAttack()
{
	if(GetStateSystem()->IsActionable(EPRAction::Action_Attack) == true)
	{
		GetStateSystem()->SetCanCancelAction(true);
		GetStateSystem()->SetActionable(EPRAction::Action_Move, false);		// 공격 시 이동 입력으로 공격 애니메이션과 별개로 이동하지 못하도록 설정합니다.
		GetStateSystem()->SetActionable(EPRAction::Action_Attack, false); 		// 공격 애니메이션에서 AnimNotify로 다음 공격 애니메이션 또는 초기 공격 애니메이션의 재생의 입력을 받도록 설정합니다.
		GetStateSystem()->SetActionable(EPRAction::Action_Dodge, true);		// 공격 도중 공격을 캔슬하고 회피할 수 있도록 설정합니다.
	
		// TargetingSystem의 동적 CameraLock을 활성화합니다.
		GetTargetingSystem()->SetActivateDynamicCameraLock(true);
	
		// AttackMode에 해당하는 NormalAttackPRAnimMontage를 설정합니다.
		FPRAnimMontage NewPlayNormalAttackPRAnimMontage = FPRAnimMontage();
		switch(AttackMode)
		{
		case EPRAttackMode::AttackMode_General:
			GetWeaponSystem()->DrawWeapon(static_cast<int32>(EPRAttackMode::AttackMode_General));
		
			// 지상 일반 공격
			if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_Grounded) == true)
			{
				NewPlayNormalAttackPRAnimMontage = GeneralNormalPRComboAnimMontage.GetPlayPRAnimMontage();
			}
			// 공중 일반 공격
			else if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
			{
				PR_LOG_SCREEN("GeneralMode NormalAttack In Air");
			}
			break;
		case EPRAttackMode::AttackMode_Awakening:
			GetWeaponSystem()->DrawWeapon(static_cast<int32>(EPRAttackMode::AttackMode_Awakening));
		
			// 지상 일반 공격
			if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_Grounded) == true)
			{
				NewPlayNormalAttackPRAnimMontage = AwakeningNormalPRComboAnimMontage.GetPlayPRAnimMontage();
			}
			// 공중 일반 공격
			else if(GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
			{
				PR_LOG_SCREEN("AwakeningMode NormalAttack In Air");
			}
			break;
		default:
			return;
		}
	
		RotationAutoTargetDirection();
		GetAnimSystem()->PlayPRAnimMontage(NewPlayNormalAttackPRAnimMontage);
		IncreasePlayNormalAttackIndex();
	}
}
#pragma endregion 

#pragma region DodgeAttack
void APRPlayerCharacter_Kyle::SetActivateDodgeAttack(bool bActivate)
{
	UPRSkill_Kyle_DodgeAttack* DodgeAttackSkill = Cast<UPRSkill_Kyle_DodgeAttack>(GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_DodgeAttack));
	if(IsValid(DodgeAttackSkill) == true)
	{
		DodgeAttackSkill->SetActivateSkill(bActivate);
	}
}
#pragma endregion

#pragma region SkillPalette
void APRPlayerCharacter_Kyle::ActivateBattleSkill(EPRCommandSkill NewPRCommandSkill)
{
	if(NewPRCommandSkill == EPRCommandSkill::CommandSkill_Ultimate)
	{
		switch(AttackMode)
		{
		case EPRAttackMode::AttackMode_General:
			Super::ActivateBattleSkill(EPRCommandSkill::CommandSkill_GeneralUltimate);
			return;
		case EPRAttackMode::AttackMode_Awakening:
			Super::ActivateBattleSkill(EPRCommandSkill::CommandSkill_AwakeningUltimate);
			return;
		default:
			Super::ActivateBattleSkill(NewPRCommandSkill);
			return;
		}
	}
	
	Super::ActivateBattleSkill(NewPRCommandSkill);
}
#pragma endregion

#pragma region ChargedAttack
// void APRPlayerCharacter_Kyle::ExecuteChargedAttack()
// {
// 	if(GetStateSystem()->IsAttackable() == true)
// 	{
// 		GetStateSystem()->SetMoveable(false);			// 공격 시 이동 입력으로 공격 애니메이션과 별개로 이동하지 못하도록 설정합니다.
// 		GetStateSystem()->SetAttackable(false);			// 공격 애니메이션에서 AnimNotify로 다음 공격 애니메이션 또는 초기 공격 애니메이션의 재생의 입력을 받도록 설정합니다.
// 		GetStateSystem()->SetDodgeable(true);			// 공격 도중 공격을 캔슬하고 회피할 수 있도록 설정합니다.
// 	
// 		// TargetingSystem의 동적 CameraLock을 활성화합니다.
// 		GetTargetingSystem()->SetActivateDynamicCameraLock(true);
// 	
// 		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
// 		if(IsValid(PlayerController) == true)
// 		{
// 			if(bExecuteChargedAttack == false)
// 			{
// 				bExecuteChargedAttack = true;
// 		
// 				GetAnimSystem()->PlayPRAnimMontage(ChargedAttackPRAnimMontage);
// 			}
// 		}
// 	}
//
// 	// HoldInput
// 	APRPlayerController* PRPlayerController = Cast<APRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
// 	if(IsValid(PRPlayerController) == true)
// 	{
// 		PRPlayerController->SetExecuteHoldInput(false);
// 	}
// }
#pragma endregion 

#pragma region Ultimate
#pragma endregion 
