// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRMeleeWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/PRStateSystemComponent.h"

APRMeleeWeapon::APRMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Debug
	bActivateTraceDebug = false;
	
	// TracePoint
	MainWeaponTraceLineGenerateInfo.Empty();
	MainWeaponTraceLines.Empty();
	
	// Trace
	MainWeaponHitActors.Empty();
	
	// Effect
	HitEffect = nullptr;
	HitEffectRotator = FRotator(90.0f, 0.0f, 0.0f);
	HitEffectScale = FVector(1.0f);
}

void APRMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	MainWeaponTraceLines.Append(GenerateTraceLines(MainWeaponMesh, MainWeaponTraceLineGenerateInfo));
}

void APRMeleeWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

#pragma region Debug
void APRMeleeWeapon::UpdateTraceDebug()
{
	for(const FPRTraceLine& NewTraceLine : MainWeaponTraceLines)
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
TArray<FPRTraceLine> APRMeleeWeapon::GenerateTraceLines(UStaticMeshComponent* NewWeaponStaticMesh, const TArray<FPRTraceLineGenerateInfo>& NewTraceLineGenerateInfo)
{
	TArray<FPRTraceLine> NewTraceLine;
	for(int Index = 0; Index < NewTraceLineGenerateInfo.Num(); Index++)
	{
		FPRTraceLine TmpTraceLine;
		TmpTraceLine.TracePoints = GenerateTracePoints(NewWeaponStaticMesh, Index, NewTraceLineGenerateInfo[Index].TopTracePointSocketName,
														NewTraceLineGenerateInfo[Index].BottomTracePointSocketName, NewTraceLineGenerateInfo[Index].TracePointGenerateCount);
		NewTraceLine.Emplace(TmpTraceLine);
	}

	return NewTraceLine;
}

TArray<FPRTracePoint> APRMeleeWeapon::GenerateTracePoints(UStaticMeshComponent* NewWeaponStaticMesh, int32 NewTraceLineIndex, FName NewTopTracePointSocketName, FName NewBottomTracePointSocketName, int32 NewTracePointGenerateCount)
{
	// 모든 소켓이 존재할 경우 TracePoint를 생성합니다.
	if(NewWeaponStaticMesh->GetSocketByName(NewTopTracePointSocketName) != nullptr
		&& NewWeaponStaticMesh->GetSocketByName(NewBottomTracePointSocketName) != nullptr)
	{
		TArray<FPRTracePoint> NewTracePoints;
		FVector NewTopTracePointRelativeLocation = NewWeaponStaticMesh->GetSocketTransform(NewTopTracePointSocketName, RTS_Component).GetLocation();
		FVector NewBottomTracePointRelativeLocation = NewWeaponStaticMesh->GetSocketTransform(NewBottomTracePointSocketName, RTS_Component).GetLocation();

		// TopTracePoint 생성
		// 생성하는 TracePoint의 이름이 겹치면 안됩니다.
		FString NewTopTracePointName = NewWeaponStaticMesh->GetName() + TEXT(" TopTracePoint");
		NewTopTracePointName.Append(FString::FromInt(NewTraceLineIndex));
		FPRTracePoint NewTopTracePoint = GenerateTracePoint(NewWeaponStaticMesh, FName(*NewTopTracePointName), NewTopTracePointRelativeLocation);
		
		if(IsValid(NewTopTracePoint.Point) == true)
		{
			NewTracePoints.Emplace(NewTopTracePoint);
		}

		// BottomTRacePoint 생성
		// 생성하는 TracePoint의 이름이 겹치면 안됩니다.
		FString NewBottomTracePointName = NewWeaponStaticMesh->GetName() + TEXT(" BottomTracePoint");
		NewBottomTracePointName.Append(FString::FromInt(NewTraceLineIndex));
		FPRTracePoint NewBottomTracePoint = GenerateTracePoint(NewWeaponStaticMesh, FName(*NewBottomTracePointName), NewBottomTracePointRelativeLocation);
		if(IsValid(NewBottomTracePoint.Point) == true)
		{
			NewTracePoints.Emplace(NewBottomTracePoint);
		}

		float OffsetBetweenTracePoints = CalculateOffsetBetweenTracePoints(NewTopTracePointRelativeLocation, NewBottomTracePointRelativeLocation, NewTracePointGenerateCount);
		for(int32 Index = 0; Index < NewTracePointGenerateCount; Index++)
		{
			// 생성하는 TracePoint의 이름이 겹치면 안됩니다.
			FString NewTracePointName = NewWeaponStaticMesh->GetName() + TEXT(" TracePoint");
			NewTracePointName.Append(FString::FromInt(NewTraceLineIndex) + "_" + FString::FromInt(Index));
		
			FPRTracePoint NewTracePoint = GenerateTracePoint(NewWeaponStaticMesh, FName(*NewTracePointName),
																CalculateTracePointSpawnOffsetLocation(NewTopTracePointRelativeLocation,
																								NewBottomTracePointRelativeLocation,
																								OffsetBetweenTracePoints * (Index + 1)));
			if(IsValid(NewTracePoint.Point) == true)
			{
				NewTracePoints.Emplace(NewTracePoint);
			}
		}

		return NewTracePoints;
	}

	return TArray<FPRTracePoint>();
}

FPRTracePoint APRMeleeWeapon::GenerateTracePoint(UStaticMeshComponent* NewWeaponStaticMesh, FName NewTracePointName, FVector NewTracePointRelativeLocation)
{
	UBoxComponent* NewPoint = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), NewTracePointName);
	if(IsValid(NewPoint) == true)
	{
		NewPoint->RegisterComponent();
		NewPoint->AttachToComponent(NewWeaponStaticMesh, FAttachmentTransformRules::KeepRelativeTransform);
		NewPoint->SetRelativeLocation(NewTracePointRelativeLocation);
		NewPoint->SetBoxExtent(FVector(4.0f, 4.0f, 4.0f));
		NewPoint->SetCollisionProfileName(TEXT("NoCollision"));
		NewPoint->PrimaryComponentTick.bCanEverTick = false;		// Tick을 사용하지 않습니다.

		FPRTracePoint NewTracePoint = FPRTracePoint(NewPoint, FVector::ZeroVector);
		return NewTracePoint;
	}

	return FPRTracePoint();
}

FVector APRMeleeWeapon::CalculateTracePointSpawnOffsetLocation(FVector TracePointFrom, FVector TracePointTo, int32 NewOffsetBetweenTracePoints) const
{
	FVector NewLocation = FVector::ZeroVector;

	if(TracePointFrom.Y > TracePointTo.Y)
	{
		NewLocation = TracePointTo;
		NewLocation.Y += NewOffsetBetweenTracePoints;
	}
	else
	{
		NewLocation = TracePointFrom;
		NewLocation.Y -= NewOffsetBetweenTracePoints;
	}

	return NewLocation;
}

float APRMeleeWeapon::CalculateOffsetBetweenTracePoints(FVector TracePointFrom, FVector TracePointTo, int32 NewGenerateTracePointCount) const
{
	/**
	 * 	TracePointLocationFrom(x), TracePointLocationTo(y), NewGenerateTracePointCount = 5 일 경우
	 * 	OffsetBetweenTracePoint = (x - y).Y / (CreateTracePointCount - 1)
	 */

	if(NewGenerateTracePointCount == 0)
	{
		return 0.0f;
	}

	const FVector TracePointLocationFrom = TracePointFrom;
	const FVector TracePointLocationTo = TracePointTo;
	const FVector NewLocation = TracePointLocationFrom - TracePointLocationTo;

	return NewLocation.Y / (NewGenerateTracePointCount + 1);
}

TArray<FPRTraceLine> APRMeleeWeapon::GetMainWeaponTraceLines() const
{
	return MainWeaponTraceLines;
}

TArray<FPRTraceLine>& APRMeleeWeapon::GetMainWeaponTraceLinesReference()
{
	return MainWeaponTraceLines;
}
#pragma endregion 

#pragma region Trace
void APRMeleeWeapon::ActivateMainWeaponAttackTraceLine(bool bNewActivateTraceDebug)
{
	ActivateAttackTraceLine(MainWeaponTraceLines, MainWeaponHitActors, bNewActivateTraceDebug);
	UpdateTracePointsPreviousLocation(MainWeaponTraceLines);
}

void APRMeleeWeapon::UpdateTracePointsPreviousLocation(TArray<FPRTraceLine>& NewTraceLines)
{
	for(FPRTraceLine& NewTraceLine : NewTraceLines)
	{
		for(FPRTracePoint& NewTracePoint : NewTraceLine.TracePoints)
		{
			NewTracePoint.UpdatePreviousLocation();
		}
	}
}

void APRMeleeWeapon::ActivateSphereAttackTraceByOwner(EPRWeaponPosition NewWeaponPosition, TMap<AActor*, bool>& NewHitActors, float NewSphereTraceSize, bool bNewActivateTraceDebug)
{
	if(IsValid(GetPROwner()) == true)
	{
		TArray<FHitResult> HitResults;
		bool bIsHit = false;
		const FVector TraceStart = GetPROwner()->GetActorLocation();
		const FVector TraceEnd = GetPROwner()->GetActorLocation();
			
		// 무기 소유자를 Trace 대상에서 제외합니다.
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Emplace(GetPROwner());

		// Debug 실행을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bNewActivateTraceDebug)
		{
			DebugType = EDrawDebugTrace::ForOneFrame;
		}

		// 무기 소유자의 타입에 해당하는 SphereTrace를 실행합니다.
		switch(GetPROwnerType())
		{
		case EPROwnerType::OwnerType_Player:
			// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
			bIsHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceStart, TraceEnd, NewSphereTraceSize, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3),
																true, ActorsToIgnore, DebugType, HitResults, true);
			break;
		case EPROwnerType::OwnerType_Enemy:
			// ECC_GameTraceChannel4는 TraceChannels의 AIAttack을 나타냅니다.
			bIsHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceStart, TraceEnd, NewSphereTraceSize, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4),
																true, ActorsToIgnore, DebugType, HitResults, true);
			break;
		case EPROwnerType::OwnerType_None:
		default:
			return;
		}

		// Hit된 액터들에게 대미지를 줍니다.
		if(bIsHit)
		{
			for(FHitResult Hit : HitResults)
			{
				if(Hit.Actor.IsValid() == true && IsHitActor(NewHitActors, *Hit.GetActor()) == false)
				{
					AActor* HitActor = Hit.GetActor();
					NewHitActors.Emplace(HitActor, false);

					if(IsTakeDamageActor(NewHitActors, *HitActor) == false)
					{
						ApplyDamage(NewHitActors, HitActor);
						SpawnHitEffectByWeaponPosition(NewWeaponPosition, Hit.ImpactPoint);
					}
				}
			}
		}
	}
}

void APRMeleeWeapon::ClearHitActors(TMap<AActor*, bool>& NewHitActors)
{
	NewHitActors.Empty();
}

void APRMeleeWeapon::ActivateAttackTraceLine(TArray<FPRTraceLine> NewTraceLines, TMap<AActor*, bool>& NewHitActors, bool bNewActivateTraceDebug)
{
	for(const FPRTraceLine& NewTraceLine : NewTraceLines)
	{
		for(const FPRTracePoint& NewTracePoint : NewTraceLine.TracePoints)
		{
			TArray<FHitResult> HitResults;
			bool bIsHit = false;
			const FVector TraceStart = NewTracePoint.PreviousLocation;
			const FVector TraceEnd = NewTracePoint.Point->GetComponentLocation();

			// 무기 소유자를 Trace 대상에서 제외합니다.
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Emplace(GetPROwner());

			// Debug 실행을 설정합니다.
			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
			if(bNewActivateTraceDebug || bActivateTraceDebug)
			{
				DebugType = EDrawDebugTrace::ForDuration;
			}

			// 무기 소유자의 타입에 해당하는 LineTrace를 실행합니다.
			switch(GetPROwnerType())
			{
			case EPROwnerType::OwnerType_Player:
				// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
				bIsHit = UKismetSystemLibrary::LineTraceMulti(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3),
																true, ActorsToIgnore, DebugType, HitResults, true);
				break;
			case EPROwnerType::OwnerType_Enemy:
				// ECC_GameTraceChannel4는 TraceChannels의 AIAttack을 나타냅니다.
				bIsHit = UKismetSystemLibrary::LineTraceMulti(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4),
																true, ActorsToIgnore, DebugType, HitResults, true);
				break;
			case EPROwnerType::OwnerType_None:
			default:
				return;
			}

			// Hit된 액터들에게 대미지를 줍니다.
			if(bIsHit)
			{
				for(FHitResult Hit : HitResults)
				{
					if(Hit.Actor.IsValid() == true && IsHitActor(NewHitActors, *Hit.GetActor()) == false)
					{
						AActor* HitActor = Hit.GetActor();
						NewHitActors.Emplace(HitActor, false);

						if(IsTakeDamageActor(NewHitActors, *HitActor) == false)
						{
							ApplyDamage(NewHitActors, HitActor);

							SpawnHitEffect(NewTracePoint.Point->GetAttachParent()->GetComponentRotation(), Hit.ImpactPoint);
						}
					}
				}
			}
		}
	}
}

bool APRMeleeWeapon::IsHitActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const
{
	return NewHitActors.Contains(&NewActor);
}

bool APRMeleeWeapon::IsTakeDamageActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const
{
	return *NewHitActors.Find(&NewActor);
}

void APRMeleeWeapon::ApplyDamage(TMap<AActor*, bool>& NewHitActors, AActor* NewHitActor)
{
	FDamageEvent DamageEvent;
	const float TotalDamage = WeaponAttackPoint + GetPROwner()->GetStatSystem()->GetCharacterStat().AttackPoint;

	// UGameplayStatics::ApplyDamage(NewHitActor, TotalDamage, GetPROwner()->GetController(), GetPROwner(), nullptr);
	// NewHitActor->TakeDamage(TotalDamage, DamageEvent, GetPROwner()->GetController(), GetPROwner());

	if(NewHitActor->GetClass()->ImplementsInterface(UInterface_PRDamageable::StaticClass()))
	{
		FPRDamageInfo DamageInfo;
		DamageInfo.Amount = TotalDamage;
		DamageInfo.DamageType = EPRDamageType::DamageType_Melee;
		DamageInfo.DamageResponse = EPRDamageResponse::DamageResponse_HitReaction;

		bool bWasDamaged = IInterface_PRDamageable::Execute_TakeDamage(NewHitActor, DamageInfo);
		if(bWasDamaged && bActivateTraceDebug)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), "Hit Actor: " + NewHitActor->GetName() + ", GetDamage: " + FString::SanitizeFloat(TotalDamage),
									true, false, FLinearColor::Red, 5.0f);
		}
	}
	
	NewHitActors.Emplace(NewHitActor, true);

	// if(bActivateTraceDebug)
	// {
	// 	UKismetSystemLibrary::PrintString(GetWorld(), "Hit Actor: " + NewHitActor->GetName() + ", GetDamage: " + FString::SanitizeFloat(TotalDamage),
	// 							true, false, FLinearColor::Red, 5.0f);
	// }
}

TMap<AActor*, bool>& APRMeleeWeapon::GetMainWeaponHitActors()
{
	return MainWeaponHitActors;
}
#pragma endregion 

#pragma region Effect
void APRMeleeWeapon::SpawnHitEffectByWeaponPosition(EPRWeaponPosition NewWeaponPosition, FVector NewHitLocation)
{
	// SubWeapon은 DualMeleeWeapon에서 override한 함수에서 사용합니다.
	if(NewWeaponPosition == EPRWeaponPosition::WeaponPosition_SubWeapon)
	{
		return;
	}
	
	if(HitEffect != nullptr && IsValid(GetPROwner()) == true)
	{
		FRotator EffectRotator = FRotator::ZeroRotator;
		FRotator WeaponRotator = GetMainWeaponMesh()->GetAttachParent()->GetComponentRotation();
		if(NewWeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
		{
			EffectRotator = FRotator(WeaponRotator.Pitch, WeaponRotator.Yaw, 0.0f);
		}

		GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(HitEffect, NewHitLocation, EffectRotator + HitEffectRotator, HitEffectScale);
		// UNiagaraComponent* SpawnHitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, NewHitLocation, EffectRotator + HitEffectRotator, HitEffectScale);
	}
}

void APRMeleeWeapon::SpawnHitEffect(FRotator NewWeaponRotator, FVector NewHitLocation)
{
	if(HitEffect != nullptr && IsValid(GetPROwner()) == true)
	{
		GetPROwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(HitEffect, NewHitLocation, NewWeaponRotator + HitEffectRotator, HitEffectScale);
		// UNiagaraComponent* SpawnHitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, NewHitLocation, NewWeaponRotator + HitEffectRotator, HitEffectScale);
	}
}
#pragma endregion

#pragma region Test
// void UDitItHitActorComponent::HitSameSocketAtDifferentTimes()
// {
// 	if (UseKismet)
// 	{
// 		for (auto Socket : MySockets)
// 		{
// 			//Private Variables
// 			const FVector* Start = LastKnownSocketLocation.Find(Socket);
// 			const FVector End = MyPrimitive->GetSocketLocation(Socket);
// 			TArray<FHitResult> OutHits;
//
// 			if (TraceByChannelOrObjects)
// 			{
// 				switch (MyKismetTraceType)
// 				{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMulti(MyWorldContextObject, *Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits);  break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMulti(MyWorldContextObject, *Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMulti(MyWorldContextObject, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMulti(MyWorldContextObject, *Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 				}
// 			}
// 			else if (!TraceByChannelOrObjects)
// 			{
// 				switch (MyKismetTraceType)
// 				{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMultiForObjects(MyWorldContextObject, *Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits);  break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMultiForObjects(MyWorldContextObject, *Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyWorldContextObject, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMultiForObjects(MyWorldContextObject, *Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 				}
// 			}
// 			AddHitToHitArray(OutHits);
// 		}
// 	}
// 	else if (!UseKismet)
// 	{
// 		//Unfinished - requires more tweaking to optimize
// 		//for (auto Socket : MySockets)
// 		//{
// 		//	TArray<FHitResult> OutHits;
// 		//	FCollisionQueryParams TraceParams;
// 		//	FVector* TraceStart = LastKnownSocketLocation.Find(Socket);
// 		//	FVector TraceEnd = MyPrimitive->GetSocketLocation(Socket);
//
// 		//	GetWorld()->LineTraceMultiByObjectType(OutHits, *TraceStart, TraceEnd, MyObjectTypesToHit, TraceParams);
// 		//	
// 		//	GetWorld()->SweepMultiByObjectType(OutHits, *TraceStart, TraceEnd, SweepQuaternion, MyObjectTypesToHit, MyCollisionShape, TraceParams);
// 		//	AddHitToHitArray(OutHits);
// 		//}
// 	}
// }
//
// void UDitItHitActorComponent::HitOtherSocketsAtSameTime()
// {
// 	//UE_LOG(LogTemp, Warning, TEXT("7"));
// 	if (UseKismet)
// 	{
// 		for (auto Socket1 : MySockets)
// 		{
// 			for (auto Socket2 : MySockets)
// 			{
// 				//Private Variables
// 				const FVector Start = MyPrimitive->GetSocketLocation(Socket1);
// 				const FVector End = MyPrimitive->GetSocketLocation(Socket2);
// 				TArray<FHitResult> OutHits;
//
// 				if (TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 						case EKismetTraceType::LineTrace:
// 						{
// 							UKismetSystemLibrary::LineTraceMulti(MyWorldContextObject, Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							AddHitToHitArray(OutHits); break;
// 						}
//
// 						case EKismetTraceType::BoxTrace:
// 						{
// 							UKismetSystemLibrary::BoxTraceMulti(MyWorldContextObject, Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
//
// 						case EKismetTraceType::CapsuleTrace:
// 						{
// 							UKismetSystemLibrary::CapsuleTraceMulti(MyWorldContextObject, Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
//
// 						case EKismetTraceType::SphereTrace:
// 						{
// 							UKismetSystemLibrary::SphereTraceMulti(MyWorldContextObject, Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
// 					}
// 				}
// 				else if (!TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 						case EKismetTraceType::LineTrace:
// 						{
// 							UKismetSystemLibrary::LineTraceMultiForObjects(MyWorldContextObject, Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							AddHitToHitArray(OutHits); break;
// 						}
//
// 						case EKismetTraceType::BoxTrace:
// 						{
// 							UKismetSystemLibrary::BoxTraceMultiForObjects(MyWorldContextObject, Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
//
// 						case EKismetTraceType::CapsuleTrace:
// 						{
// 							UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyWorldContextObject, Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
//
// 						case EKismetTraceType::SphereTrace:
// 						{
// 							UKismetSystemLibrary::SphereTraceMultiForObjects(MyWorldContextObject, Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 							break;
// 						}
// 					}
// 				}
// 			AddHitToHitArray(OutHits);
// 			}
// 		}
// 	}
// 	else if (!UseKismet) {
// 		for (auto Socket1 : MySockets)
// 		{
// 			for (auto Socket2 : MySockets)
// 			{
//
// 				//Not optimized:
// 				//TArray<FHitResult> OutHits;
// 				//FCollisionQueryParams TraceParams = MyTraceParams;
// 				//FVector TraceStart = MyPrimitive->GetSocketLocation(Socket1);
// 				//FVector TraceEnd = MyPrimitive->GetSocketLocation(Socket2);
// 				//TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToHit = MyObjectTypesToHit;
//
// 				//GetWorld()->LineTraceMultiByObjectType(OutHits, TraceStart, TraceEnd, ObjectTypesToHit, TraceParams);
//
// 				//AddHitToHitArray(OutHits);
// 			}
// 		}
// 	}
// }
//
// void UDitItHitActorComponent::HitOtherSocketsAtDifferentTime()
// {
// 	if (UseKismet)
// 	{
// 		for (auto Socket1 : MySockets)
// 		{
// 			for (auto Socket2 : MySockets)
// 			{
// 				TArray<FHitResult> OutHits1;
// 				FVector Start = MyPrimitive->GetSocketLocation(Socket1);
// 				FVector* End = LastKnownSocketLocation.Find(Socket2);
//
// 				if (TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMulti(MyWorldContextObject, Start, *End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits1); break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMulti(MyWorldContextObject, Start, *End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMulti(MyWorldContextObject, Start, *End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMulti(MyWorldContextObject, Start, *End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 					}
// 				}
// 				else if (!TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMultiForObjects(MyWorldContextObject, Start, *End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits1); break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMultiForObjects(MyWorldContextObject, Start, *End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyWorldContextObject, Start, *End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMultiForObjects(MyWorldContextObject, Start, *End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 					}
// 				}
//
// 				AddHitToHitArray(OutHits1);
// 			}
//
// 			for (auto Socket3 : MySockets)
// 			{
// 				TArray<FHitResult> OutHits2;
// 				FVector* Start = LastKnownSocketLocation.Find(Socket3);
// 				FVector End = MyPrimitive->GetSocketLocation(Socket1);
//
// 				if (TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMulti(MyWorldContextObject, *Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits2); break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMulti(MyWorldContextObject, *Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMulti(MyWorldContextObject, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMulti(MyWorldContextObject, *Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 					}
// 				}
// 				else if (!TraceByChannelOrObjects)
// 				{
// 					switch (MyKismetTraceType)
// 					{
// 					case EKismetTraceType::LineTrace:
// 					{
// 						UKismetSystemLibrary::LineTraceMultiForObjects(MyWorldContextObject, *Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						AddHitToHitArray(OutHits2); break;
// 					}
//
// 					case EKismetTraceType::BoxTrace:
// 					{
// 						UKismetSystemLibrary::BoxTraceMultiForObjects(MyWorldContextObject, *Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::CapsuleTrace:
// 					{
// 						UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyWorldContextObject, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
//
// 					case EKismetTraceType::SphereTrace:
// 					{
// 						UKismetSystemLibrary::SphereTraceMultiForObjects(MyWorldContextObject, *Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
// 						break;
// 					}
// 					}
// 				}
//
// 				AddHitToHitArray(OutHits2);
// 			}
// 		}
// 	}
// 	else if (!UseKismet)
// 	{
// 		/*for (auto Socket1 : MySockets)
// 		{
// 			for (auto Socket2 : MySockets)
// 			{
// 				TArray<FHitResult> OutHits1;
// 				FCollisionQueryParams TraceParams;
// 				FVector TraceStart = MyPrimitive->GetSocketLocation(Socket1);
// 				FVector* TraceEnd = LastKnownSocketLocation.Find(Socket2);
//
// 				GetWorld()->LineTraceMultiByObjectType(OutHits1, TraceStart, *TraceEnd, MyObjectTypesToHit, TraceParams);
//
// 				AddHitToHitArray(OutHits1);
// 			}
//
// 			for (auto Socket3 : MySockets)
// 			{
// 				TArray<FHitResult> OutHits2;
// 				FCollisionQueryParams TraceParams;
// 				FVector* TraceStart = LastKnownSocketLocation.Find(Socket3);
// 				FVector TraceEnd = MyPrimitive->GetSocketLocation(Socket1);
//
// 				GetWorld()->LineTraceMultiByObjectType(OutHits2, *TraceStart, TraceEnd, MyObjectTypesToHit, TraceParams);
//
// 				AddHitToHitArray(OutHits2);
// 			}
// 		}*/
// 	}
// }
#pragma endregion 