// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

APRBaseCharacter::APRBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 94.0f);

	// Pawn
	// 컨트롤러가 회전할 때 캐리겉가 같이 회전하지 않도록 설정합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// SkeletalMesh
	// 기본 스켈레탈 메시를 설정합니다.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -96.0f), FRotator(0.0f, 270.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mannequin(TEXT("/Game/Characters/Mannequin/Character/Mesh/SK_Mannequin"));
	if(SK_Mannequin.Succeeded() == true)
	{
		GetMesh()->SetSkeletalMesh(SK_Mannequin.Object);
	}

	// TakeDamage
	HitAnimMontage = FPRAnimMontage();
	
	// MovementInput
	bJumped = false;

	// AnimSystem
	AnimSystem = CreateDefaultSubobject<UPRAnimSystemComponent>(TEXT("AnimSystem"));

	// MovementSystem
	MovementSystem = CreateDefaultSubobject<UPRMovementSystemComponent>(TEXT("MovementSystem"));

	// StateSystem
	StateSystem = CreateDefaultSubobject<UPRStateSystemComponent>(TEXT("StateSystem"));
	
	// StatSystem
	StatSystem = CreateDefaultSubobject<UPRStatSystemComponent>(TEXT("StatSystem"));

	// WeaponSystem
	WeaponSystem = CreateDefaultSubobject<UPRWeaponSystemComponent>(TEXT("WeaponSystem"));

	// SkillSystem
	SkillSystem = CreateDefaultSubobject<UPRSkillSystemComponent>(TEXT("SkillSystem"));

	// ObjectPoolSystem
	ObjectPoolSystem = CreateDefaultSubobject<UPRObjectPoolSystemComponent>(TEXT("ObjectPoolSystem"));

	// EffectSystem
	EffectSystem = CreateDefaultSubobject<UPREffectSystemComponent>(TEXT("EffectSystem"));

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = true;								// 캐릭터가 이동하는 방향으로 회전합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);	// 캐릭터의 회전속도입니다.
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	GetCharacterMovement()->AirControl = 0.15f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 180.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;		// 기본 값은 2.0f입니다. MovementSystem의 PredictStopLocation을 사용하기 위해서 1.0f으로 설정합니다.

	// Dodge
	DodgePRAnimMontages.Empty();
	ForwardDodgePRAnimMontageID = 0;
	BackwardDodgePRAnimMontageID = 0;
	LeftDodgePRAnimMontageID = 0;
	RightDodgePRAnimMontageID = 0;
	AerialForwardDodgePRAnimMontageID = 0;
	
	// Effect
	SignatureEffectColor = FLinearColor(20.0f, 15.0f, 200.0f, 1.0f);
}

void APRBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetStatSystem()->OnHealthPointIsZeroDelegate.AddUFunction(this, FName("Dead"));
}

void APRBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMovementSystem()->SetAllowedGait(EPRGait::Gait_Running);
	GetMovementSystem()->SetRotationMode(EPRRotationMode::RotationMode_VelocityDirection);

	// Dodge
	InitializeDodgePRAnimMontages();
}

void APRBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APRBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	GetMovementSystem()->OnCharacterMovementModeChanged(GetCharacterMovement()->MovementMode);
}

void APRBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if(GetWorld()->GetTimerManager().IsTimerActive(JumpedDelayTimerHandle) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpedDelayTimerHandle);
	}
	
	bJumped = false;
}

float APRBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	TakeHit(DamageCauser);
	GetStatSystem()->TakeDamage(FinalDamage);
	
	return FinalDamage;
}

#pragma region TakeDamage
bool APRBaseCharacter::IsDead() const
{
	return GetStateSystem()->IsDead();
}

void APRBaseCharacter::TakeHit(AActor* DamageCauser)
{
	StateSystem->SetIsHit(true);

	// 대미지를 준 액터를 바라봅니다
	if(IsValid(DamageCauser) == true)
	{
		const FVector DamageCauserReverseForwardVector = DamageCauser->GetActorForwardVector() * -1.0f;
		const FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(DamageCauserReverseForwardVector);
		SetActorRotation(LookAtRotation);

		APRPlayerCharacter* PRPlayerDamageCauser = Cast<APRPlayerCharacter>(DamageCauser);
		if(PRPlayerDamageCauser != nullptr)
		{
			PRPlayerDamageCauser->UpdateComboCount();
		}
	}

	// GetAnimSystem()->PlayPRAnimMontage(HitAnimMontage);
	PlayAnimMontage(HitAnimMontage.AnimMontage);
}

void APRBaseCharacter::Dead()
{
	GetStateSystem()->SetIsDead(true);
	SetActorEnableCollision(false);
}
#pragma endregion 

#pragma region MovementInput
bool APRBaseCharacter::IsJumped() const
{
	return bJumped;
}

void APRBaseCharacter::Jump()
{
	Super::Jump();
	
	if(GetStateSystem()->IsActionable(EPRAction::Action_Move) == true && GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == false)
	{
		bJumped = true;
	
		if(GetWorld()->GetTimerManager().IsTimerActive(JumpedDelayTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(JumpedDelayTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bJumped = false;
	
				GetWorld()->GetTimerManager().ClearTimer(JumpedDelayTimerHandle);
			}), 0.1f, false);
		}
	}
}
#pragma endregion

#pragma region AnimSystem
void APRBaseCharacter::InitializePRAnimMontages()
{
}
#pragma endregion 

#pragma region WeaponSystem
// FPRWeaponGroup APRBaseCharacter::GetEquippedWeaponGroup() const
// {
// 	return GetWeaponSystem()->GetEquippedWeaponGroup();
// }
#pragma endregion 

#pragma region Dodge
void APRBaseCharacter::InitializeDodgePRAnimMontages()
{
	DodgePRAnimMontages = GetAnimSystem()->GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToMap(ForwardDodgePRAnimMontageID, AerialForwardDodgePRAnimMontageID);

}

void APRBaseCharacter::Dodge()
{
	// 하위 클래스에서 오버라이딩합니다.
}
#pragma endregion 

#pragma region NormalAttack
void APRBaseCharacter::InitializePlayNormalAttackIndex()
{
}

void APRBaseCharacter::IncreasePlayNormalAttackIndex()
{
}
#pragma endregion

#pragma region Effect
FLinearColor APRBaseCharacter::GetSignatureEffectColor() const
{
	return SignatureEffectColor;
}
#pragma endregion 

