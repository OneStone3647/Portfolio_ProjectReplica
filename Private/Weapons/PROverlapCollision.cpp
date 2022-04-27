// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PROverlapCollision.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Weapons/PRBaseWaepon.h"
#include "Components/BoxComponent.h"

APROverlapCollision::APROverlapCollision()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// PRCharacterReference
	PROwner = nullptr;

	// Debug
	bDrawDebug = false;
	DrawDebugTime = 1.0f;
	DrawDebugPointSize = 20.0f;

	// OverlapCollision
	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	OverlapCollision->SetupAttachment(RootComponent);
	OverlapCollision->SetGenerateOverlapEvents(false);
	OverlapCollision->SetCollisionProfileName("OverlapCollision");
}

void APROverlapCollision::BeginPlay()
{
	Super::BeginPlay();

	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &APROverlapCollision::AttackOverlap);

	DrawDebugOverlapCollision(bDrawDebug);
}

#pragma region Debug
void APROverlapCollision::DrawDebugOverlapCollision(bool bFlag)
{
	OverlapCollision->SetHiddenInGame(!bFlag);
}

void APROverlapCollision::SetDrawDebug(bool bFlag)
{
	bDrawDebug = bFlag;
}
#pragma endregion 

#pragma region CharacterReference
void APROverlapCollision::InitializePROwner(APRBaseCharacter* PRBaseCharacter)
{
	if(IsValid(PRBaseCharacter) == true)
	{
		PROwner = PRBaseCharacter;
	}
}
#pragma endregion 

void APROverlapCollision::ExecuteOverlap(bool bFlag)
{
	OverlapCollision->SetGenerateOverlapEvents(bFlag);
}

bool APROverlapCollision::IsExecuteOverlap() const
{
	return OverlapCollision->GetGenerateOverlapEvents();
}

void APROverlapCollision::ClearHitActors()
{
	HitActors.Empty();
}

void APROverlapCollision::AttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(SweepResult.Actor.IsValid() == true && SweepResult.GetActor() != PROwner && SweepResult.GetActor() != this)
	{
		AActor* OverlapActor = SweepResult.GetActor();

		HitActors.Add(OverlapActor, false);

		if(bDrawDebug)
		{
			PR_LOG_SCREEN("OverlapActor: %s", *OverlapActor->GetName());
		}

		APRBaseWaepon* PRWeapon = PROwner->GetWeaponSystem()->GetEquipWeapon();
		if(IsValid(PRWeapon) ==  true)
		{
			PRWeapon->ExecuteHitParticleEffect(OverlapActor, OverlapActor->GetActorLocation(), PRWeapon->GetActorRotation());
		}

		ApplyDamage(OverlapActor);
	}
}

bool APROverlapCollision::IsHitActor(const AActor* HitedActor) const
{
	return HitActors.Contains(HitedActor);
}

bool APROverlapCollision::IsTakeDamageActor(const AActor* HitedActor) const
{
	return *HitActors.Find(HitedActor);
}

void APROverlapCollision::ApplyDamage(AActor* HitedActor)
{
	FDamageEvent DamageEvent;
	float TotalDamage = 0.0f;
	APRBaseWaepon* PRWeapon = PROwner->GetWeaponSystem()->GetEquipWeapon();
	if(IsValid(PRWeapon) ==  true)
	{
		TotalDamage = PRWeapon->GetWeaponDamage() + PROwner->GetStatSystem()->GetCharacterStat().AttackPoint;
	}
	else
	{
		TotalDamage = PROwner->GetStatSystem()->GetCharacterStat().AttackPoint;
	}

	HitedActor->TakeDamage(TotalDamage, DamageEvent, PROwner->GetController(), PROwner);
	HitActors.Add(HitedActor, true);

	if(bDrawDebug)
	{
		PR_LOG_SCREEN("Attack Actor: %s, HitActor: %s, HitDamage: %f", *PROwner->GetName(), *HitedActor->GetName(), TotalDamage);
	}
}

