// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRJudgementCutArea.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APRJudgementCutArea::APRJudgementCutArea()
{
	// DamageArea
	DamageArea = CreateDefaultSubobject<USphereComponent>(TEXT("DamageArea"));
	DamageArea->SetupAttachment(RootComponent);
	DamageArea->SetCollisionProfileName("NoCollision");
	
	// JudgementCutEffect
	JudgementCutEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JudgementCutEffect"));
	JudgementCutEffect->SetupAttachment(DamageArea);

	HitSoundSource = nullptr;
	HitSounds.Empty();
	bActivateDebug = false;
	Damage = 5.0f;
	DamageInterval = 0.1f;
	// TimeStopTimeDilation = 0.1f;
	bActivateDamageArea = false;
	// CooldownElapsed = 0.0f;
	MaxDamageCount = 4;
	DamageCount = 0;
	NearestTarget = nullptr;
	SpawnRadius = 300.0f;
	DefaultSpawnDistance = 500.0f;
	FindFloorRange = 500.0f;
	TargetPullForce = 10.0f;
}

void APRJudgementCutArea::BeginPlay()
{
	Super::BeginPlay();

	// HitSound를 생성합니다.
	if(HitSoundSource != nullptr)
	{
		for(int32 Index = 0; Index < MaxDamageCount; Index++)
		{
			HitSounds.Emplace(GenerateHitSound(HitSoundSource, Index));
		}
	}
}

// void APRJudgementCutArea::UpdatePooledObject_Implementation(float DeltaTime)
// {
// 	if(GetObjectOwner() != nullptr)
// 	{
// 		CustomTimeDilation = GetObjectOwner()->CustomTimeDilation;
// 	}
//
// 	// Effect 최신화
// 	if(JudgementCutEffect != nullptr && JudgementCutEffect->IsActive() == true)
// 	{
// 		JudgementCutEffect->AdvanceSimulation(1, DeltaTime);
// 	}
//
// 	if(CustomTimeDilation != 1.0f)
// 	{
// 		// TimeStop이 실행 중일 때
// 		UpdateDamageArea(DeltaTime * TimeStopTimeDilation);
// 	}
// 	else
// 	{
// 		// 일반상태일 때
// 		UpdateDamageArea(DeltaTime);
// 	}
// }

void APRJudgementCutArea::InitializeSpawnLocation_Implementation()
{
	NearestTarget = nullptr;
	
	if(IsValid(GetObjectOwner()) == true)
	{
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetObjectOwner());
		if(PRPlayerCharacter != nullptr && PRPlayerCharacter->GetTargetingSystem() != nullptr)
		{
			UPRTargetingSystemComponent* TargetSystem = PRPlayerCharacter->GetTargetingSystem();
			if(TargetSystem->IsActivateLockOnTarget() == true && IsValid(TargetSystem->GetLockedOnTarget()) == true)
			{
				// Target을 LockOn하고 있을 때
				SetActorLocation(FindFloorLocation(TargetSystem->GetLockedOnTarget()));
				return;
			}
			
			// Target을 LockOn하고 있지 않을 때
			TArray<FHitResult> HitResults;
			bool bIsHit = false;
				
			// 오브젝트 소유자를 Trace 대상에서 제외합니다.
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Emplace(GetObjectOwner());
	
			// Debug 실행을 설정합니다.
			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
			if(bActivateDebug)
			{
				DebugType = EDrawDebugTrace::ForDuration;
			}
				
			const FVector TraceStart = PRPlayerCharacter->GetActorLocation();
			const FVector TraceEnd = TraceStart + (PRPlayerCharacter->GetActorForwardVector() * TargetSystem->GetMaxSearchTargetalbeDistance());

			// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
			bIsHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceStart, TraceEnd, SpawnRadius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3),
												false, ActorsToIgnore, DebugType, HitResults, true);
			if(bIsHit)
			{
				// 가장 가까운 Target을 탐색합니다.
				for(FHitResult Hit : HitResults)
				{
					if(Hit.Actor.IsValid() == true)
					{
						if(NearestTarget != nullptr)
						{
							AActor* NewNearestTarget = GetNearestTarget(NearestTarget, Hit.GetActor());
							if(NewNearestTarget != nullptr)
							{
								NearestTarget = NewNearestTarget;
							}
						}
						else
						{
							NearestTarget = Hit.GetActor();
						}
					}
				}

				// 가장 가까운 Target의 바닥으로 위치를 옮깁니다.
				if(NearestTarget != nullptr)
				{
					SetActorLocation(FindFloorLocation(NearestTarget));
					return;
				}
			}

			if(PRPlayerCharacter->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
			{
				// PlayerCharacter가 공중에 존재할 경우
				SetActorLocation(PRPlayerCharacter->GetActorLocation() + GetObjectOwner()->GetActorForwardVector() * DefaultSpawnDistance);
			}
			else
			{
				// PlayerCharacter가 공중에 존재하지 않을 경우
				SetActorLocation(FindFloorLocation(GetObjectOwner()) + (GetObjectOwner()->GetActorForwardVector() * DefaultSpawnDistance));
			}
		}
	}
}

void APRJudgementCutArea::Activate_Implementation()
{
	Super::Activate_Implementation();

	// CooldownElapsed = 0.0f;
	DamageCount = 0;
	JudgementCutEffect->Activate(true);
	bActivateDamageArea = true;

	GetWorld()->GetTimerManager().SetTimer(DamageAreaTimerHandle, this, &APRJudgementCutArea::ActivateDamageArea, DamageInterval, true);
}

void APRJudgementCutArea::ActivateDamageArea()
{
	TArray<FHitResult> HitResults;
	bool bIsHit = false;

	// 오브젝트 소유자를 Trace 대상에서 제외합니다.
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(GetObjectOwner());
	
	// Debug 실행을 설정합니다.
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if(bActivateDebug)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}

	// HitSound를 재생합니다.
	if(GetHitSound(DamageCount) != nullptr)
	{
		GetHitSound(DamageCount)->Play();
	}
	
	DamageCount++;

	// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
	bIsHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), DamageArea->GetScaledSphereRadius(),
													UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), false, ActorsToIgnore, DebugType,
													HitResults, true);

	// Hit된 액터들에게 대미지를 줍니다.
	APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(GetObjectOwner());
	if(bIsHit && IsValid(PRCharacter) == true)
	{
		for(FHitResult Hit : HitResults)
		{
			if(Hit.Actor.IsValid() == true)
			{
				AActor* HitActor = Hit.GetActor();
				UGameplayStatics::ApplyDamage(HitActor, Damage, PRCharacter->GetController(), PRCharacter, nullptr);
				// Hit된 액터를 DamageArea로 끌어 당깁니다.
				PullTargetToDamageArea(HitActor);
			}
		}
	}

	// 최대 횟수만큼 대미지를 줬으면 DamageArea를 비활성화합니다.
	if(DamageCount == MaxDamageCount)
	{
		bActivateDamageArea = false;
		DamageCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DamageAreaTimerHandle);
	}
}

// void APRJudgementCutArea::UpdateDamageArea(float DeltaTime)
// {
// 	if(bActivateDamageArea)
// 	{
// 		CooldownElapsed += DeltaTime;
//
// 		// 재사용 대기시간이 대미지를 주는 시간의 간격보다 클 경우 DamageArea에 존재하는 Target들에게 대미지를 줍니다.
// 		if(CooldownElapsed >= DamageInterval)
// 		{
// 			CooldownElapsed = 0;
// 			ActivateDamageArea();
// 		}
//
// 		// 최대 횟수만큼 대미지를 줬으면 DamageArea를 비활성화합니다.
// 		if(DamageCount == MaxDamageCount)
// 		{
// 			bActivateDamageArea = false;
// 			DamageCount = 0;
// 			CooldownElapsed = 0;
// 		}
// 	}
// }

FVector APRJudgementCutArea::FindFloorLocation(AActor* Target) const
{
	if(Target != nullptr)
	{
		FHitResult HitResult;
		bool bIsHit = false;
		const FVector TraceStart = Target->GetActorLocation();
		const FVector TraceEnd = TraceStart + (Target->GetActorUpVector() * FMath::Abs(FindFloorRange) * -1.0f);

		// 오브젝트 소유자를 Trace 대상에서 제외합니다.
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Emplace(GetObjectOwner());
		ActorsToIgnore.Emplace(Target);

		// Debug 실행을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bActivateDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}
	
		bIsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Visibility),
														false, ActorsToIgnore, DebugType, HitResult, true);
		if(bIsHit)
		{
			return HitResult.ImpactPoint;
		}
	}
	
	return FVector::ZeroVector;
}

AActor* APRJudgementCutArea::GetNearestTarget(AActor* TargetA, AActor* TargetB)
{
	if(GetObjectOwner() != nullptr && TargetA != nullptr && TargetB != nullptr)
	{
		// TargetA의 거리
		FVector NewObjectOwnerLocation = FVector(GetObjectOwner()->GetActorLocation().X, GetObjectOwner()->GetActorLocation().Y, 0.0f);
		FVector NewTargetALocation = FVector(TargetA->GetActorLocation().X, TargetA->GetActorLocation().Y, 0.0f);
		float DistanceToTargetA = UKismetMathLibrary::Vector_Distance(NewObjectOwnerLocation, NewTargetALocation);

		// TargetB의 거리
		FVector NewTargetBLocation = FVector(TargetB->GetActorLocation().X, TargetB->GetActorLocation().Y, 0.0f);
		float DistanceToTargetB = UKismetMathLibrary::Vector_Distance(NewObjectOwnerLocation, NewTargetBLocation);

		if(DistanceToTargetA < DistanceToTargetB)
		{
			return TargetA; 
		}
		else
		{
			return TargetB;
		}
	}

	return nullptr;
}

void APRJudgementCutArea::PullTargetToDamageArea(AActor* Target)
{
	if(Target != nullptr)
	{
		FVector NewTargetLocation = FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0.0f);
		FVector NewDamageAreaLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f);
		FVector DeltaLocation = NewTargetLocation - NewDamageAreaLocation;
		UKismetMathLibrary::Vector_Normalize(DeltaLocation);

		Target->AddActorLocalOffset(DeltaLocation * TargetPullForce);
	}
}

UAudioComponent* APRJudgementCutArea::GenerateHitSound(USoundBase* SoundAsset, int32 GenerateIndex)
{
	if(SoundAsset != nullptr)
	{
		// GenerateIndex에 해당하는 Component의 이름 생성
		FString Index = FString::Printf(TEXT("%d"), GenerateIndex);
		FString NewComponentName = "HitSound" + Index;
		FName ComponentName = FName(*NewComponentName);
	
		UAudioComponent* NewHitSound = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass(), ComponentName);
		if(IsValid(NewHitSound) == true)
		{
			NewHitSound->RegisterComponent();
			NewHitSound->SetSound(SoundAsset);
			NewHitSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			NewHitSound->SetRelativeLocation(FVector::ZeroVector);
			NewHitSound->SetAutoActivate(false);

			return NewHitSound;
		}
	}

	return nullptr;
}

UAudioComponent* APRJudgementCutArea::GetHitSound(int32 Index) const
{
	if(HitSounds.IsValidIndex(Index) == true)
	{
		return HitSounds[Index];
	}

	return nullptr;
}
