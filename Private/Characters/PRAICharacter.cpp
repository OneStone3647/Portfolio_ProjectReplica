// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PRAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/PRTargetAimSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widgets/PRHealthPointBarWidget.h"

APRAICharacter::APRAICharacter()
{
	// CapsuleComponent
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AICharacter"));	
	
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
}

void APRAICharacter::BeginPlay()
{
	Super::BeginPlay();

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
}

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
#pragma endregion 
