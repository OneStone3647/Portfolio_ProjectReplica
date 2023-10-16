// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRBaseWeapon.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRAICharacter.h"
#include "Components/ArrowComponent.h"

APRBaseWeapon::APRBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// WeaponInfo
	bActivate = false;
	WeaponAttackPoint = 10.0f;
	PROwner = nullptr;
	PROwnerType = EPROwnerType::OwnerType_None;
	bIsDraw = false;
	bIsSheath = false;

	// MainWeapon
	MainWeapon = CreateDefaultSubobject<USceneComponent>(TEXT("MainWeapon"));
	MainWeapon->SetupAttachment(Root);
	MainWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainWeaponMesh"));
	MainWeaponMesh->SetupAttachment(MainWeapon);
	MainWeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	MainWeaponProjectileSpawnLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("MainWeaponProjectileSpawnLocation"));
	MainWeaponProjectileSpawnLocation->SetupAttachment(MainWeaponMesh);
	// MainWeaponInfo = FPRWeaponInfo();
	MainWeaponDrawSocketName = FName();
	MainWeaponSheathSocketName = FName();
}

void APRBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void APRBaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Debug
	UpdateTraceDebug();
}

void APRBaseWeapon::InitializeWeapon(APRBaseCharacter* NewPROwner)
{
	if(IsValid(NewPROwner) == true)
	{
		SetPROwner(NewPROwner);
		if(NewPROwner->IsA(APRPlayerCharacter::StaticClass()) == true)
		{
			PROwnerType = EPROwnerType::OwnerType_Player;
		}
		else if(NewPROwner->IsA(APRAICharacter::StaticClass()) == true)
		{
			PROwnerType = EPROwnerType::OwnerType_Enemy;
		}
		else
		{
			PROwnerType = EPROwnerType::OwnerType_None;
		}
	}
}

void APRBaseWeapon::Draw()
{
	if(IsValid(GetPROwner()) == true)
	{
		bIsDraw = true;
		bIsSheath = false;
		
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules);
		MainWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, MainWeaponDrawSocketName);
	}
}

void APRBaseWeapon::Sheath()
{
	if(IsValid(GetPROwner()) == true)
	{
		bIsDraw = false;
		bIsSheath = true;
		
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules);
		MainWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, MainWeaponSheathSocketName);
	}
}

#pragma region Debug
void APRBaseWeapon::UpdateTraceDebug()
{
}

void APRBaseWeapon::SetActivateTraceDebug(bool bNewActivateTraceDebug)
{
	bActivateTraceDebug = bNewActivateTraceDebug;
}
#pragma endregion 

#pragma region WeaponInfo
bool APRBaseWeapon::IsActivate() const
{
	return bActivate;
}

void APRBaseWeapon::SetActivate(bool bNewActivate)
{
	bActivate = bNewActivate;
	SetActorHiddenInGame(!bActivate);
	
	// 비활성화 시 부착한 액터에서 떼어냅니다.
	if(!bActivate)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

bool APRBaseWeapon::IsDraw() const
{
	return bIsDraw;
}

bool APRBaseWeapon::IsSheath() const
{
	return bIsSheath;
}

float APRBaseWeapon::GetWeaponAttackPoint() const
{
	return WeaponAttackPoint;
}

APRBaseCharacter* APRBaseWeapon::GetPROwner() const
{
	return PROwner;
}

void APRBaseWeapon::SetPROwner(APRBaseCharacter* NewPROwner)
{
	PROwner = NewPROwner;
}

EPROwnerType APRBaseWeapon::GetPROwnerType() const
{
	return PROwnerType;
}

UStaticMeshComponent* APRBaseWeapon::GetMainWeaponMesh() const
{
	return MainWeaponMesh;
}

FName APRBaseWeapon::GetMainWeaponDrawSocketName() const
{
	return MainWeaponDrawSocketName;
}

FName APRBaseWeapon::GetMainWeaponSheathSocketName() const
{
	return MainWeaponSheathSocketName;
}

UArrowComponent* APRBaseWeapon::GetMainWeaponProjectileSpawnLocation() const
{
	return MainWeaponProjectileSpawnLocation;
}
#pragma endregion 

#pragma region SubWeapon
UStaticMeshComponent* APRBaseWeapon::GetSubWeaponMesh() const
{
	return nullptr;
}

UArrowComponent* APRBaseWeapon::GetSubWeaponProjectileSpawnLocation() const
{
	return nullptr;
}
#pragma endregion 

