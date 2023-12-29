// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter.h"

#include "AnimInstances/PRBaseAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/PRTargetAimSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widgets/PRHealthPointBarWidget.h"
#include "Objects/PRAISpawner.h"

APRAICharacter::APRAICharacter()
{
	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AICharacter"));

	// CharacterMovement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// AIActivate
	bActivate = false;
	Lifespan = 10.0f;

	// TimeStop
	bTimeStopActive = false;
	
	// TargetAimSystem
	TargetAimSystem = CreateDefaultSubobject<UPRTargetAimSystemComponent>(TEXT("TargetAimSystem"));
	TargetAimSystem->SetupAttachment(RootComponent);

	// HealthPointBar
	HealthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthPointBar"));
	HealthPointBar->SetupAttachment(GetMesh());
	HealthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HealthPointBar->SetWidgetSpace(EWidgetSpace::World);
	HealthPointBar->SetDrawSize(FVector2D(100.0f, 30.0f));
	HealthPointBarVisibleTimeSinceTakeHit = 3.0f;

	// PoolIndex
	PoolIndex = 0;
}

void APRAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// HealthPoint Widget 초기화
	InitializeHealthPointBar();
}

void APRAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HealthPointBarLookAtPlayerCamera();
}

void APRAICharacter::TakeHit(AActor* DamageCauser)
{
	Super::TakeHit(DamageCauser);

	if(HealthPointBar->bHiddenInGame)
	{
		HealthPointBar->SetHiddenInGame(false);
	}

	// 이전 피격에서 실행한 TimerHandle을 초기화합니다.
	if(GetWorld()->GetTimerManager().IsTimerActive(HealthPointBarHiddenTimerHandle) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthPointBarHiddenTimerHandle);
	}

	// Target으로 지정되지 않았을 경우
	if(TargetAimSystem->GetLockOnWidget()->bHiddenInGame)
	{
		GetWorld()->GetTimerManager().SetTimer(HealthPointBarHiddenTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if(GetStateSystem()->IsHit() == true)
			{
				HealthPointBar->SetHiddenInGame(true);
			}

			GetWorld()->GetTimerManager().ClearTimer(HealthPointBarHiddenTimerHandle);
		}), HealthPointBarVisibleTimeSinceTakeHit, false);
	}	
}

void APRAICharacter::Dead()
{
	Super::Dead();

	TargetAimSystem->GetLockOnWidget()->SetHiddenInGame(true);
	HealthPointBar->SetHiddenInGame(true);

	if(Lifespan > 0)
	{
		// 타이머로 AI 캐릭터가 사망한 후 수명이 다하면 AI 캐릭터를 비활성화합니다.
		FTimerHandle DeactivateDelayTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DeactivateDelayTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			Deactivate();
		}), Lifespan, false);
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 Deactivate 함수를 실행합니다.
		Deactivate();
	}
}

#pragma region Activate
bool APRAICharacter::IsActivate() const
{
	return bActivate;
}
void APRAICharacter::Activate()
{
	bActivate = true;
	SetActorHiddenInGame(false);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AICharacter"));

	// FootIk를 활성화합니다.
	UPRBaseAnimInstance* PRAnimInstance = Cast<UPRBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if(IsValid(PRAnimInstance))
	{
		PRAnimInstance->ActivateFootIK();
	}
}

void APRAICharacter::Deactivate()
{
	SetActorHiddenInGame(true);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	// FootIk를 비활성화합니다.
	UPRBaseAnimInstance* PRAnimInstance = Cast<UPRBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if(IsValid(PRAnimInstance))
	{
		PRAnimInstance->DeactivateFootIK();
	}
	
	if(IsValid(GetWeaponSystem()->GetEquippedWeapon()))
	{
		GetWeaponSystem()->GetEquippedWeapon()->SetActivate(false);
	}

	if(OnAIDeactivate.IsBound())
	{
		OnAIDeactivate.Broadcast(this);
	}
	
	if(OnDynamicAIDeactivate.IsBound())
	{
		OnDynamicAIDeactivate.Broadcast(this);
	}

	bActivate = false;
}
#pragma endregion

#pragma region TimeStop
bool APRAICharacter::IsTimeStopActive() const
{
	return bTimeStopActive;
}

void APRAICharacter::TimeStopActive()
{
	bTimeStopActive = true;
	GetEffectSystem()->TimeStopActive();
	GetObjectPoolSystem()->TimeStopActive();
}

void APRAICharacter::TimeStopDeactive()
{
	bTimeStopActive = false;
	GetEffectSystem()->TimeStopDeactive();
	GetObjectPoolSystem()->TimeStopDeactive();
}
#pragma endregion 

#pragma region TargetAimSystem
void APRAICharacter::LockedOnTarget(bool bActivateLockedOn)
{
	TargetAimSystem->GetLockOnWidget()->SetHiddenInGame(!bActivateLockedOn);
	if(bActivateLockedOn)
	{
		TargetAimSystem->SetCollisionProfileName("LockOnTarget");
	}
	else
	{
		TargetAimSystem->SetCollisionProfileName("Target");
	}
	HealthPointBar->SetHiddenInGame(!bActivateLockedOn);
}

FVector APRAICharacter::GetTargetAimLocation() const
{
	return TargetAimSystem->GetComponentLocation();
}
#pragma endregion 

#pragma region HealthPointBar
void APRAICharacter::InitializeHealthPointBar()
{
	if(HealthPointBar != nullptr)
	{
		HealthPointBar->SetHiddenInGame(true);

		UPRHealthPointBarWidget* NewHealthPointBar = Cast<UPRHealthPointBarWidget>(HealthPointBar->GetUserWidgetObject());
		if(NewHealthPointBar != nullptr && GetStatSystem() != nullptr)
		{
			NewHealthPointBar->BindStatSystem(GetStatSystem());
		}
	}
}

void APRAICharacter::HealthPointBarLookAtPlayerCamera()
{
	if(!HealthPointBar->bHiddenInGame)
	{
		APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if(PlayerCameraManager != nullptr)
		{
			FRotator NewHealthPointBarRotator = UKismetMathLibrary::MakeRotationFromAxes(PlayerCameraManager->GetTransformComponent()->GetForwardVector() * -1.0f,
																							PlayerCameraManager->GetTransformComponent()->GetRightVector() * -1.0f,
																							PlayerCameraManager->GetTransformComponent()->GetUpVector());
			HealthPointBar->SetWorldRotation(NewHealthPointBarRotator);
		}
	}
}

UWidgetComponent* APRAICharacter::GetHealthPointBar() const
{
	return HealthPointBar;
}

APRAISpawner* APRAICharacter::GetSpawner() const
{
	return Spawner;
}

void APRAICharacter::SetSpawner(APRAISpawner* NewSpawner)
{
	Spawner = NewSpawner;
}
#pragma endregion 

#pragma region PoolIndex
int32 APRAICharacter::GetPoolIndex() const
{
	return PoolIndex;
}

void APRAICharacter::SetPoolIndex(int32 NewAIListIndex)
{
	PoolIndex = NewAIListIndex;
}
#pragma endregion 

#pragma region AI
void APRAICharacter::DefaultAttack()
{
	if(AttackAnimMontage != nullptr)
	{
		// GetMesh()->GetAnimInstance()->Montage_Play(AttackAnimMontage);

		UPRBaseAnimInstance* AnimInstance = Cast<UPRBaseAnimInstance>(GetMesh()->GetAnimInstance());
		if(AnimInstance != nullptr)
		{
			// AnimInstance->PlayAnAnimMontage(AttackAnimMontage);
		}
	}
}
#pragma endregion 
