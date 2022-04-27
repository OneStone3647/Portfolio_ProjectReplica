// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRBaseWaepon.h"
#include "Weapons/PRTraceCollision.h"
#include "Characters/PRAICharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APRBaseWaepon::APRBaseWaepon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// WeaponStaticMesh
	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->SetupAttachment(Root);
	WeaponStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// Debug
	bDrawDebug = false;
	DrawDebugTime = 1.0f;
	DrawDebugTracePointSize = 20.0f;

	// WeaponInfo
	bIsActive = false;
	PROwner = nullptr;
	OwnerType = EPROwnerType::OwnerType_None;
	WeaponEquipType = EPRWeaponEquipType::WeaponEquipType_Single;

	// WeaponPRAnimMontage
	WeaponPRAnimMontageDataTable = nullptr;
	WeaponPRAnimMontageArray.Empty();
	WeaponPRAnimMontageIndex = 0;

	// Effect
	HitParticleEffect = nullptr;
	HitParticleEffectScale = FVector(0.5f, 0.5f, 0.5f);
	SpawnWeaponEffect = nullptr;
	SpawnWeaponEffectColor = FLinearColor(20.000017f, 14.999998, 200.0, 1.0f);
	
	// TraceCollision
	BaseTraceCollision = CreateDefaultSubobject<UPRTraceCollision>(TEXT("TraceCollision"));
	BaseTraceCollision->SetupAttachment(Root);
	TraceCollisionsArray.Empty();

	// Trace
	ClearHitActors();
}

void APRBaseWaepon::BeginPlay()
{
	Super::BeginPlay();

	InitializeWeaponPRAnimMontageArray();
	AddAllTraceCollisionToArray();
	SetWeaponActive(false);
	
	// Debug
	DrawDebug(bDrawDebug);
}

#pragma region Debug
void APRBaseWaepon::DrawDebug(bool bFlag)
{
	for(UPRTraceCollision* TraceCollision : TraceCollisionsArray)
	{
		TraceCollision->DrawDebugTraceCollision(bFlag);
	}
}

bool APRBaseWaepon::IsDrawDebug() const
{
	return bDrawDebug;
}
#pragma endregion 

#pragma region WeaponInfo
void APRBaseWaepon::SetWeaponActive(bool bFlag, bool bSpawnEffectFlag)
{
	if(bSpawnEffectFlag)
	{
		ExecuteSpawnWeaponEffect();
	}
	
	bIsActive = bFlag;
	SetActorHiddenInGame(!bIsActive);
	SetActorTickEnabled(bIsActive);
}

bool APRBaseWaepon::IsWeaponActive() const
{
	return bIsActive;
}

void APRBaseWaepon::SetPROnwerAndOwnerType(APRBaseCharacter* NewPROwner)
{
	if(IsValid(NewPROwner) == true)
	{
		SetPROwner(NewPROwner);

		if(PROwner->IsA(APRPlayerCharacter::StaticClass()) == true)
		{
			OwnerType = EPROwnerType::OwnerType_Player;
			return;
		}

		if(PROwner->IsA(APRAICharacter::StaticClass()) == true)
		{
			OwnerType = EPROwnerType::OwnerType_AI;
			return;
		}
	}

	NewPROwner = nullptr;
	OwnerType = EPROwnerType::OwnerType_None;
}

bool APRBaseWaepon::IsEqualWeaponEquipType(EPRWeaponEquipType NewPRWeaponEquipType) const
{
	return WeaponEquipType == NewPRWeaponEquipType;
}

void APRBaseWaepon::SetPROwner(APRBaseCharacter* NewPROwner)
{
	if(IsValid(NewPROwner) == true)
	{
		PROwner = NewPROwner;
	}
	else
	{
		PROwner = nullptr;
	}
}

void APRBaseWaepon::SetOwnertype(EPROwnerType NewOwnerType)
{
	OwnerType = NewOwnerType;
}

EPRWeaponEquipType APRBaseWaepon::GetWeaponEquipType() const
{
	return WeaponEquipType;
}

float APRBaseWaepon::GetWeaponDamage() const
{
	return WeaponDamage;
}
#pragma endregion

#pragma region WeaponPRAnimMontage
void APRBaseWaepon::InitializeWeaponPRAnimMontageIndex()
{
	WeaponPRAnimMontageIndex = 0;
}

void APRBaseWaepon::IncreaseWeaponPRAnimMontageIndex()
{
	WeaponPRAnimMontageIndex = WeaponPRAnimMontageIndex++;
	if(WeaponPRAnimMontageIndex == WeaponPRAnimMontageArray.Num())
	{
		InitializeWeaponPRAnimMontageIndex();
	}
}

void APRBaseWaepon::InitializeWeaponPRAnimMontageArray()
{
	TArray<FPRAnimMontage> NewWeaponPRAnimMontageArray;
	if(WeaponPRAnimMontageDataTable)
	{
		TArray<FName> RowNames = WeaponPRAnimMontageDataTable->GetRowNames();
		for(int32 Index = 0; Index < RowNames.Num(); Index++)
		{
			FPRAnimMontage DataTableRow = *(WeaponPRAnimMontageDataTable->FindRow<FPRAnimMontage>(RowNames[Index], FString("")));
			WeaponPRAnimMontageArray.Add(DataTableRow);
		}
	}
	
	InitializeWeaponPRAnimMontageIndex();
}

int32 APRBaseWaepon::GetWeaponPRAnimMontageIndex() const
{
	return WeaponPRAnimMontageIndex;
}
#pragma endregion 

#pragma region Effect
void APRBaseWaepon::ExecuteHitParticleEffect(AActor* HitActor, FVector HitLocation, FRotator WeaponRotator)
{
	APRBaseCharacter* HitPRCharacter = Cast<APRBaseCharacter>(HitActor);
	if(IsValid(HitPRCharacter) == true && HitParticleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(HitParticleEffect, HitPRCharacter->GetMesh(), NAME_None, HitLocation, WeaponRotator,
												HitParticleEffectScale, EAttachLocation::KeepWorldPosition);
	}
}

void APRBaseWaepon::ExecuteSpawnWeaponEffect()
{
	if(SpawnWeaponEffect)
	{
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnWeaponEffect, GetActorLocation(), GetActorRotation());
		UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(SpawnNiagaraComponent, "WeaponStaticMesh", WeaponStaticMesh);

		SpawnNiagaraComponent->SetVariableLinearColor("LinearColor", SpawnWeaponEffectColor);
	}
}
#pragma endregion 

#pragma region TraceCollision
void APRBaseWaepon::UpdateAllTraceCollisionsLocation()
{
	for(UPRTraceCollision* TraceCollision : TraceCollisionsArray)
	{
		TraceCollision->UpdateTraceCollisionsLocation();
	}
}

void APRBaseWaepon::AddAllTraceCollisionToArray()
{
	TArray<USceneComponent*> ChildrenComponentsArray;

	// @bIncludeAllDescendants 모든 하위 항목을 Children에 포함할지 나타내는 매개변수입니다. 
	GetRootComponent()->GetChildrenComponents(false, ChildrenComponentsArray); 	// 자식 컴포넌트들을 ChildrenComponentsArray로 가져옵니다.

	for(USceneComponent* ChildrenComponent : ChildrenComponentsArray)
	{
		UPRTraceCollision* TraceCollision = Cast<UPRTraceCollision>(ChildrenComponent);
		if(TraceCollision)
		{
			TraceCollisionsArray.AddUnique(TraceCollision);
		}
	}
}
#pragma endregion 

#pragma region Trace
void APRBaseWaepon::ExecuteAttackTrace()
{
	for(UPRTraceCollision* TraceCollision : TraceCollisionsArray)
	{
		for(const FPRTraceCollisionStruct PRTraceCollisionStruct: TraceCollision->GetTraceCollisionStructArray())
		{
			TArray<FHitResult> HitResults;
			const FVector TraceStart = PRTraceCollisionStruct.PreviousLocation;
			const FVector TraceEnd = PRTraceCollisionStruct.TraceCollision->GetComponentLocation();
			bool bIsHit = false;

			// Debug
			if(bDrawDebug)
			{
				DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, DrawDebugTime);
			}

			switch(OwnerType)
			{
			case EPROwnerType::OwnerType_Player:
				// ECollisionChannel::ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
				bIsHit = GetWorld()->LineTraceMultiByChannel(HitResults, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel3);
				break;
			case EPROwnerType::OwnerType_AI:
				// ECollisionChannel::ECC_GameTraceChannel4는 TraceChannels의 AIAttack을 나타냅니다.
				bIsHit = GetWorld()->LineTraceMultiByChannel(HitResults, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4);
				break;
			default:
				break;
			}

			if(bIsHit)
			{
				for(FHitResult Hit : HitResults)
				{
					if(Hit.Actor.IsValid() == true && IsHitActor(Hit.GetActor()) == false)
					{
						AActor* HitActor = Hit.GetActor();
						HitActors.Add(HitActor, false);
					
						if(IsTakeDamageActor(HitActor) == false)
						{
							// Debug
							if(bDrawDebug)
							{
								DrawDebugPoint(GetWorld(), Hit.Location, DrawDebugTracePointSize, FColor::Red, false, DrawDebugTime);
							}

							ExecuteHitParticleEffect(HitActor, Hit.Location, GetActorRotation());
							ApplyDamage(HitActor);
						}
					}
				}
			}
		}

		TraceCollision->UpdateTraceCollisionsLocation();
	}
}

void APRBaseWaepon::ClearHitActors()
{
	HitActors.Empty();
}

bool APRBaseWaepon::IsHitActor(const AActor* HitedActor) const
{
	return HitActors.Contains(HitedActor);
}

bool APRBaseWaepon::IsTakeDamageActor(const AActor* HitedActor) const
{
	return *HitActors.Find(HitedActor);
}

void APRBaseWaepon::ApplyDamage(AActor* HitedActor)
{
	FDamageEvent DamageEvent;
	const float TotalDamage = WeaponDamage + PROwner->GetStatSystem()->GetCharacterStat().AttackPoint;

	HitedActor->TakeDamage(TotalDamage, DamageEvent, PROwner->GetController(), PROwner);
	HitActors.Add(HitedActor, true);

	if(bDrawDebug)
	{
		PR_LOG_SCREEN("Attack Actor: %s, HitActor: %s, HitDamage: %f", *PROwner->GetName(), *HitedActor->GetName(), TotalDamage);
	}
}
#pragma endregion 

