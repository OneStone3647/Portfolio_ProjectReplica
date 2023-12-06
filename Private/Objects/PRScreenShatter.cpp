// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRScreenShatter.h"
#include "DestructibleComponent.h"
#include "Components/SceneCaptureComponent2D.h"

APRScreenShatter::APRScreenShatter()
{
	// CaptureComponent2D
	GetCaptureComponent2D()->bCaptureOnMovement = false;
	
	// Destructible
	Destructible = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible"));
	Destructible->SetupAttachment(RootComponent);
	Destructible->SetEnableGravity(false);
	BaseDamage = 1000.0f;
	HurtOrigin = FVector::ZeroVector;
	DamageRadius = 20.0f;
	ImpulseStrength = 400.0f;
	bFullDamage = false;
	EnableGravityDelay = 0.2f;
	DestroyDelay = 5.0f;

	// ChromaKey
	ChromaKey = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChromaKey"));
	ChromaKey->SetupAttachment(RootComponent);
}

void APRScreenShatter::BeginPlay()
{
	Super::BeginPlay();

	// Destructible에 대미지를 가합니다.
	HurtOrigin = GetDestructible()->GetComponentLocation();
	GetDestructible()->ApplyRadiusDamage(BaseDamage, HurtOrigin, DamageRadius, ImpulseStrength, bFullDamage);

	// EnableGravityDelay 후 Destructible의 중력을 설정합니다.
	FTimerHandle EnableGravityTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(EnableGravityTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		GetDestructible()->SetEnableGravity(true);
	}), EnableGravityDelay, false);

	// DestroyDelay 후 이 액터를 소멸시킵니다.
	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		Destroy();
	}), DestroyDelay, false);
}
