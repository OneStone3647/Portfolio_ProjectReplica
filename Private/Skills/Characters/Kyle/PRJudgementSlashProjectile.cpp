// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRJudgementSlashProjectile.h"
#include "NiagaraSystem.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/PRBaseWeapon.h"

APRJudgementSlashProjectile::APRJudgementSlashProjectile()
{
	ImpactNiagaraEffect = nullptr;
	ImpactNiagaraEffectRotation = FRotator(0.0f, 90.0f, 0.0f);
	ImpactSound = nullptr;

	// ProjectileSetting
	ProjectileSpeed = 2000.0f;
	ProjectileGravityScale = 0.0f;
	ProjectileDamage = 20.0f;
	
	// Debug
	bActivateDebug = false;

	// HitBox
	HitActors.Empty();
}

void APRJudgementSlashProjectile::SpawnImpactNiagaraEffect(FVector Location, FRotator Rotation)
{
	if(IsValid(GetObjectOwner()) == true)
	{
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetObjectOwner());
		if(PRPlayerCharacter != nullptr)
		{
			FRotator OwnerForwardRotation = UKismetMathLibrary::Conv_VectorToRotator(PRPlayerCharacter->GetActorForwardVector());
			// Roll을 조절하여 투사체의 각도에 맞게 회전합니다.
			// Yaw를 조절하여 캐릭터의 전방을 바라보게 회전합니다.
			FRotator NewEffectRotation = FRotator(GetActorRotation().Roll * -1.0f, OwnerForwardRotation.Yaw, 0.0f);
			NewEffectRotation += Rotation;
			
			PRPlayerCharacter->GetEffectSystem()->SpawnNiagaraEffectAtLocation(ImpactNiagaraEffect, Location, NewEffectRotation);
		}
	}
}

void APRJudgementSlashProjectile::UpdatePooledObject_Implementation(float DeltaTime)
{
	Super::UpdatePooledObject_Implementation(DeltaTime);

	if(IsValid(GetObjectOwner()) == true)
	{
		// 오브젝트 소유자의 커스텀 시간 흐름 속도를 동기화합니다.
		CustomTimeDilation = GetObjectOwner()->CustomTimeDilation;
	}
}

void APRJudgementSlashProjectile::Deactivate_Implementation()
{
	ClearHitActors(HitActors);
	
	Super::Deactivate_Implementation();
}

void APRJudgementSlashProjectile::InitializeSpawnLocation_Implementation()
{
	Super::InitializeSpawnLocation_Implementation();

	if(IsValid(GetObjectOwner()) == true)
	{
		APRBaseCharacter* PRBaseCharacterOwner = Cast<APRBaseCharacter>(GetObjectOwner());
		if(PRBaseCharacterOwner != nullptr)
		{
			APRBaseWeapon* EquippedWeapon = PRBaseCharacterOwner->GetWeaponSystem()->GetEquippedWeapon();
			if(EquippedWeapon != nullptr)
			{
				// 오브젝트 소유자의 전방으로 나아가도록 회전 값을 설정합니다.
				const FRotator OwnerForwardRotation = UKismetMathLibrary::Conv_VectorToRotator(PRBaseCharacterOwner->GetActorForwardVector());
				// 무기를 휘두르는 각도에 따라 오브젝트가 나아가도록 보이게 회전 값을 설정합니다.
				const FRotator WeaponRotation = EquippedWeapon->GetMainWeaponProjectileSpawnLocation()->GetComponentRotation();
				const FRotator NewRotation = FRotator(OwnerForwardRotation.Pitch, OwnerForwardRotation.Yaw, WeaponRotation.Roll);

				SetActorLocationAndRotation(PRBaseCharacterOwner->GetActorLocation(), NewRotation);
			}
		}
	}
}

void APRJudgementSlashProjectile::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap_Implementation(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(IsValid(GetObjectOwner()) == true)
	{
		TArray<FHitResult> HitResults;
		bool bIsHit = false;
		const FVector TraceStart = GetActorLocation();
		const FVector TraceEnd = GetActorLocation();
			
		// 무기 소유자를 Trace 대상에서 제외합니다.
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Emplace(GetObjectOwner());

		// Debug 실행을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bActivateDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}

		// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
		bIsHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), TraceStart, TraceEnd, GetHitBox()->GetScaledBoxExtent(), GetHitBox()->GetComponentRotation(),
														UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), false, ActorsToIgnore, DebugType,
														HitResults, true);

		// Hit된 액터들에게 대미지를 줍니다.
		if(bIsHit)
		{
			for(FHitResult Hit : HitResults)
			{
				if(Hit.Actor.IsValid() == true && IsHitActor(HitActors, *Hit.GetActor()) == false)
				{
					AActor* HitActor = Hit.GetActor();
					HitActors.Emplace(HitActor, false);

					// 대미지를 받은 액터인지 판별한 후 대미지를 받지 않았을 경우 대미지를 주고 ImpactNiagaraEffect를 Spawn하고 ImpactSound를 재생합니다.
					if(IsTakeDamageActor(HitActors, *HitActor) == false)
					{
						ApplyDamage(HitActors, HitActor);

						// ImpactNiagaraEffect를 Spawn합니다.
						SpawnImpactNiagaraEffect(Hit.ImpactPoint, ImpactNiagaraEffectRotation);

						// ImpactSound를 재생합니다.
						if(ImpactSound != nullptr)
						{
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.ImpactPoint);
						}
					}
				}
			}
		}
	}
}

void APRJudgementSlashProjectile::ClearHitActors(TMap<AActor*, bool>& NewHitActors)
{
	NewHitActors.Empty();
}

bool APRJudgementSlashProjectile::IsHitActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const
{
	return NewHitActors.Contains(&NewActor);
}

bool APRJudgementSlashProjectile::IsTakeDamageActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const
{
	return *NewHitActors.Find(&NewActor);
}

void APRJudgementSlashProjectile::ApplyDamage(TMap<AActor*, bool>& NewHitActors, AActor* NewHitActor)
{
	if(IsValid(GetObjectOwner()) == true)
	{
		APRBaseCharacter* PRBaseCharacterOwner = Cast<APRBaseCharacter>(GetObjectOwner());
		if(PRBaseCharacterOwner != nullptr)
		{
			FDamageEvent DamageEvent;
			const float TotalDamage = ProjectileDamage;

			UGameplayStatics::ApplyDamage(NewHitActor, TotalDamage, PRBaseCharacterOwner->GetController(), PRBaseCharacterOwner, nullptr);
			NewHitActors.Emplace(NewHitActor, true);

			if(bActivateDebug)
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Hit Actor: " + NewHitActor->GetName() + ", GetDamage: " + FString::SanitizeFloat(TotalDamage),
										true, false, FLinearColor::Red, 5.0f);
			}
		}
	}
}
