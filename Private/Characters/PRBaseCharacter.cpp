// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Components/PRDodgeSystemComponent.h"
#include "Enums/Enum_PRJump.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "Kismet/KismetMathLibrary.h"

APRBaseCharacter::APRBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(36.0f, 94.0f);

	// Pawn
	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하지 않도록 설정합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// SkeletalMesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -94.0f), FRotator(0.0f, -90.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mannequin(TEXT("/Game/Characters/Mannequin/Character/Mesh/SK_Mannequin"));
	if(SK_Mannequin.Succeeded() == true)
	{
		GetMesh()->SetSkeletalMesh(SK_Mannequin.Object);
	}

	// MovementSystem
	MovementSystem = CreateDefaultSubobject<UPRMovementSystemComponent>(TEXT("MovementSystem"));

	// StateSystem
	StateSystem = CreateDefaultSubobject<UPRStateSystemComponent>(TEXT("StateSystem"));

	// StatSystem
	StatSystem = CreateDefaultSubobject<UPRStatSystemComponent>(TEXT("StatSystem")); 

	// AnimSystem
	AnimSystem = CreateDefaultSubobject<UPRAnimSystemComponent>(TEXT("AnimSystem"));

	// WeaponSystem
	WeaponSystem = CreateDefaultSubobject<UPRWeaponSystemComponent>((TEXT("WeaponSystem")));

	// DodgeSystem
	DodgeSystem = CreateDefaultSubobject<UPRDodgeSystemComponent>(TEXT("DodgeSystem"));

	// CharacterMovement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.15f;
	GetCharacterMovement()->MaxWalkSpeed = MovementSystem->GetRunSpeed();
	GetCharacterMovement()->MinAnalogWalkSpeed = MovementSystem->GetMinAnalogWalkSpeed();

	// Audio
	FootStepAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FootStepAudio"));
	FootStepAudio->SetupAttachment(RootComponent);
	FootStepVolume = 1.0f;
	JumpAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JumpAudio"));
	JumpAudio->SetupAttachment(RootComponent);
	JumpVoiceVolume = 1.0f;
}

void APRBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatSystem->OnHealthPointIsZero.AddUFunction(this, FName("Dead"));
}

void APRBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSystem->SetRotationMode(EPRRotationMode::RotationMode_VelocityDirection);
}

void APRBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APRBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	MovementSystem->OnCharacterMovementModeChanged(GetCharacterMovement()->MovementMode);
}

void APRBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	LandedAndAccelerating();
}

float APRBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	TakeHit(DamageCauser);
	StatSystem->TakeDamage(FinalDamage);
	
	return FinalDamage;
}

bool APRBaseCharacter::IsDead() const
{
	return StateSystem->IsDead();
}

void APRBaseCharacter::TakeHit(AActor* DamageCauser)
{
	StateSystem->SetIsHit(true);
	
	// 대미지를 준 액터를 바라봅니다.
	if(IsValid(DamageCauser) == true)
	{
		const FVector DamageCauserReverseForwardVector = DamageCauser->GetActorForwardVector() * -1.0f;
		const FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(DamageCauserReverseForwardVector);

		SetActorRotation(LookAtRotation);
	}

	PlayAnimMontage(AnimSystem->GetHitAnimMontage());
}

void APRBaseCharacter::Dead()
{
	StateSystem->SetIsDead(true);
	SetActorEnableCollision(false);
}

void APRBaseCharacter::SetRotationMode(EPRRotationMode NewRotationMode)
{
	MovementSystem->SetRotationMode(NewRotationMode);
}

#pragma region CharacterMovement
void APRBaseCharacter::LandedAndAccelerating()
{
	if(MovementSystem->IsAccelerating() == true)
	{
		FHitResult HitResult;
		bool bIsHit = false;
		const FVector TraceStart = GetActorLocation();
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 150.0f);
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = true;
		
		bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, Params);
		if(bIsHit)
		{
			JumpAudio->Play();
			JumpAudio->SetIntParameter(TEXT("SurfaceType"), static_cast<int32>(UGameplayStatics::GetSurfaceType(HitResult)));
			JumpAudio->SetIntParameter(TEXT("Jump"), static_cast<int32>(EPRJump::Jump_End));
			JumpAudio->SetBoolParameter(TEXT("PlayFootStep"), true);
			JumpAudio->SetBoolParameter(TEXT("PlayVoice"), false);
			JumpAudio->SetFloatParameter(TEXT("FootStepVolume"), FootStepVolume);
			JumpAudio->SetFloatParameter(TEXT("VoiceVolume"), JumpVoiceVolume);
		}
	}
}

float APRBaseCharacter::GetFootStepVolume() const
{
	return FootStepVolume;
}

float APRBaseCharacter::GetJumpVoiceVolume() const
{
	return JumpVoiceVolume;
}

void APRBaseCharacter::SetRunOrSprint(EPRGait NewPRGait)
{
	MovementSystem->SetAllowedGait(NewPRGait);
}
#pragma endregion

#pragma region Dodge
void APRBaseCharacter::Dodge()
{
}
#pragma endregion 

#pragma region Attack
void APRBaseCharacter::Attack()
{
}
#pragma endregion
