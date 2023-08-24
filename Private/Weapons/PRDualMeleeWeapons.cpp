// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRDualMeleeWeapons.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/ArrowComponent.h"
#include "Components/PREffectSystemComponent.h"

APRDualMeleeWeapons::APRDualMeleeWeapons()
{
	// WeaponInfo
	WeaponAttackPoint = 5.0f;
	
	// SubWeapon
	SubWeapon = CreateDefaultSubobject<USceneComponent>(TEXT("SubWeapon"));
	SubWeapon->SetupAttachment(Root);
	SubWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubWeaponMesh"));
	SubWeaponMesh->SetupAttachment(SubWeapon);
	SubWeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	SubWeaponProjectileSpawnLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("SubWeaponProjectileSpawnLocation"));
	SubWeaponProjectileSpawnLocation->SetupAttachment(SubWeaponMesh);
	// SubWeaponInfo = FPRWeaponInfo();
	SubWeaponDrawSocketName = FName();
	SubWeaponSheathSocketName = FName();

	// TracePoint
	SubWeaponTraceLineGenerateInfo.Empty();
	SubWeaponTraceLines.Empty();

	// Trace
	SubWeaponHitActors.Empty();
}

void APRDualMeleeWeapons::BeginPlay()
{
	Super::BeginPlay();

	SubWeaponTraceLines.Append(GenerateTraceLines(SubWeaponMesh, SubWeaponTraceLineGenerateInfo));
}

void APRDualMeleeWeapons::Draw()
{
	Super::Draw();

	if(IsValid(GetPROwner()) == true)
	{
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		SubWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, SubWeaponDrawSocketName);
	}
}

void APRDualMeleeWeapons::Sheath()
{
	Super::Sheath();

	if(IsValid(GetPROwner()) == true)
	{
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		SubWeapon->AttachToComponent(GetPROwner()->GetMesh(), AttachmentTransformRules, SubWeaponSheathSocketName);
	}
}

#pragma region SubWeapon
UStaticMeshComponent* APRDualMeleeWeapons::GetSubWeaponMesh() const
{
	return SubWeaponMesh;
}

UArrowComponent* APRDualMeleeWeapons::GetSubWeaponProjectileSpawnLocation() const
{
	return SubWeaponProjectileSpawnLocation;
}
#pragma endregion 

#pragma region Debug
void APRDualMeleeWeapons::UpdateTraceDebug()
{
	Super::UpdateTraceDebug();

	for(const FPRTraceLine& NewTraceLine : SubWeaponTraceLines)
	{
		for(const FPRTracePoint& NewTracePoint : NewTraceLine.TracePoints)
		{
			if(IsValid(NewTracePoint.Point) == true)
			{
				NewTracePoint.Point->SetHiddenInGame(!bActivateTraceDebug);
			}
		}
	}
}
#pragma endregion 

#pragma region TracePoint
TArray<FPRTraceLine> APRDualMeleeWeapons::GetSubWeaponTraceLines() const
{
	return SubWeaponTraceLines;
}

TArray<FPRTraceLine>& APRDualMeleeWeapons::GetSubWeaponTraceLinesReference()
{
	return SubWeaponTraceLines;
}
#pragma endregion 

#pragma region Trace
void APRDualMeleeWeapons::ActivateSubWeaponAttackTraceLine(bool bNewActivateTraceDebug)
{
	ActivateAttackTraceLine(SubWeaponTraceLines, SubWeaponHitActors, bNewActivateTraceDebug);
	UpdateTracePointsPreviousLocation(SubWeaponTraceLines);
}

TMap<AActor*, bool>& APRDualMeleeWeapons::GetSubWeaponHitActors()
{
	return SubWeaponHitActors;
}
#pragma endregion 

#pragma region Effect
void APRDualMeleeWeapons::SpawnHitEffectByWeaponPosition(EPRWeaponPosition NewWeaponPosition, FVector NewHitLocation)
{
	if(HitEffect != nullptr && IsValid(GetPROwner()) == true)
	{
		FRotator EffectRotator = FRotator::ZeroRotator;
		FRotator WeaponRotator = FRotator::ZeroRotator;
		if(NewWeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
		{
			WeaponRotator = GetMainWeaponMesh()->GetAttachParent()->GetComponentRotation();
		}
		else
		{
			WeaponRotator = GetSubWeaponMesh()->GetAttachParent()->GetComponentRotation();
		}
		
		EffectRotator = FRotator(WeaponRotator.Pitch, WeaponRotator.Yaw, 0.0f);

		GetPROwner()->GetEffectSystem()->SpawnEffectAtLocation(HitEffect, NewHitLocation, EffectRotator + HitEffectRotator, HitEffectScale);
		// UNiagaraComponent* SpawnHitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, NewHitLocation, EffectRotator + HitEffectRotator, HitEffectScale);
	}
}
#pragma endregion 
