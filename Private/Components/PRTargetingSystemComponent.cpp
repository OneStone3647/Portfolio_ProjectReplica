// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRTargetingSystemComponent.h"

#include "PRPlayerController.h"
#include "Characters/PRAICharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UPRTargetingSystemComponent::UPRTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Debug
	bDrawDebug = false;
	DebugTargetSphereSize = 30.0f;

	// LockOn
	bActivateLockOnTarget = false;
	LockedOnTarget = nullptr;
	MaxSearchTargetalbeDistance = 2000.0f;
	bActivateDynamicCameraLock = false;
	StopDynamicCameraLockScreenRatio = 0.7f;
	InterpSpeed = 5.0f;
	LastTimeChangeTarget = 0.0f;
	TimeSinceChangeTarget = 0.0f;
	MinMouseValueToChangeTarget = 3.0f;
	MinDelayTimeToChangeTarget = 0.3f;
	bAnalogNeutralSinceLastSwitchTarget = false;
	MinAnalogValueToChangeTarget = 0.2f;
}

void UPRTargetingSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPRTargetingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCameraLock();
	
	if(bDrawDebug)
	{
		UpdateDebug();
	}
}

#pragma region Debug
void UPRTargetingSystemComponent::UpdateDebug()
{
	// 탐지하는 범위를 드로우 디버그합니다.
	DrawDebugSphere(GetWorld(), GetPROwner()->GetActorLocation(), MaxSearchTargetalbeDistance, 12, FColor::Green);

	// LockedOnTarget을 드로우 디버그합니다.
	if(IsValid(LockedOnTarget) == true)
	{
		APRAICharacter* TargetPRAICharacter = Cast<APRAICharacter>(LockedOnTarget);
		if(TargetPRAICharacter != nullptr)
		{
			DrawDebugSphere(GetWorld(), TargetPRAICharacter->GetTargetAimLocation(), 30.0f, 12, FColor::Red);
		}
	}

	// LockedOnTarget을 제외한 TargetableActors를 드로우 디버그합니다.
	TArray<AActor*> TargetableActors = SearchTargetableActors();
	TargetableActors.Remove(LockedOnTarget);
	for(AActor* TargetableActor : TargetableActors)
	{
		APRAICharacter* TargetablePRAICharacter = Cast<APRAICharacter>(TargetableActor);
		if(TargetablePRAICharacter != nullptr)
		{
			DrawDebugSphere(GetWorld(), TargetablePRAICharacter->GetTargetAimLocation(), 30.0f, 12, FColor::Green);
		}
	}

	// // LockedOnTarget을 제외한 TargetableActors를 드로우 디버그합니다.
	// if(!bActivateLockOnTarget)
	// {
	// 	TArray<AActor*> TargetableActors = SearchTargetableActors();
	// 	TargetableActors.Remove(LockedOnTarget);
	// 	for(AActor* TargetableActor : TargetableActors)
	// 	{
	// 		APRAICharacter* TargetablePRAICharacter = Cast<APRAICharacter>(TargetableActor);
	// 		if(TargetablePRAICharacter != nullptr)
	// 		{
	// 			DrawDebugSphere(GetWorld(), TargetablePRAICharacter->GetTargetAimLocation(), 30.0f, 12, FColor::Green);
	// 		}
	// 	}
	// }
	//
	// // LeftTargetableActors를 드로우 디버그합니다.
	// TArray<AActor*> DebugLeftTargetableActors = LeftTargetableActors;
	// DebugLeftTargetableActors.Remove(LockedOnTarget);
	// for(AActor* TargetableActor : DebugLeftTargetableActors)
	// {
	// 	APRAICharacter* TargetablePRAICharacter = Cast<APRAICharacter>(TargetableActor);
	// 	if(TargetablePRAICharacter != nullptr)
	// 	{
	// 		DrawDebugSphere(GetWorld(), TargetablePRAICharacter->GetTargetAimLocation(), 30.0f, 12, FColor::Yellow);
	// 	}
	// }
	//
	// // RightTargetableActors를 드로우 디버그합니다.
	// TArray<AActor*> DebugRightTargetableActors = RightTargetableActors;
	// DebugRightTargetableActors.Remove(LockedOnTarget);
	// for(AActor* TargetableActor : DebugRightTargetableActors)
	// {
	// 	APRAICharacter* TargetablePRAICharacter = Cast<APRAICharacter>(TargetableActor);
	// 	if(TargetablePRAICharacter != nullptr)
	// 	{
	// 		DrawDebugSphere(GetWorld(), TargetablePRAICharacter->GetTargetAimLocation(), 30.0f, 12, FColor::Blue);
	// 	}
	// }
}
#pragma endregion

bool UPRTargetingSystemComponent::IsActivateLockOnTarget() const
{
	return bActivateLockOnTarget && IsValid(LockedOnTarget) == true;
}

void UPRTargetingSystemComponent::ActivateLockOnTarget()
{
	SetLockedOnTarget(FindTarget());
	if(IsValid(LockedOnTarget) == true)
	{
		// LockedOnTarget이 존재할 경우 LockOnTarget을 향해 카메라를 고정합니다.
		EnableCameraLock();
	}
	else
	{
		// LockedOnTarget이 존재하지 않을 경우 LockOnTarget을 취소합니다.
		CancelLockOnTarget();
	}
}

void UPRTargetingSystemComponent::CancelLockOnTarget()
{
	DisableCameraLock();
	LockedOnTarget = nullptr;
	// LeftTargetableActors.Empty();
	// RightTargetableActors.Empty();
}

void UPRTargetingSystemComponent::ChangeLockOnTargetForTurnValue(EPRInputMode InputMode, float TurnValue)
{
	if(bActivateLockOnTarget && IsValid(LockedOnTarget) == true)
	{
		APRAICharacter* TargetPRAICharacter = Cast<APRAICharacter>(LockedOnTarget);
		if(TargetPRAICharacter != nullptr)
		{
			TargetPRAICharacter->LockedOnTarget(false);
		}

		switch(InputMode)
		{
		case EPRInputMode::InputMode_Mouse:
			ChangeLockOnTargetWithMouse(TurnValue);
			break;
		case EPRInputMode::InputMode_Gamepad:
			ChangeLockOnTargetWithGamepad(TurnValue);
			break;
		default:
			break;
		}

		EnableCameraLock();
	}
}

void UPRTargetingSystemComponent::UpdateCameraLock()
{
	if(bActivateLockOnTarget)
	{
		if(IsValid(LockedOnTarget) == true && GetOwner()->GetDistanceTo(LockedOnTarget) <= MaxSearchTargetalbeDistance)
		{
			APRBaseCharacter* TargetPRCharacter = Cast<APRBaseCharacter>(LockedOnTarget);
			if(TargetPRCharacter != nullptr && TargetPRCharacter->IsDead() == false)
			{
				APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				if(PlayerController != nullptr)
				{					
					APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetOwner());
					if(IsValid(PRPlayerCharacter) == true)
					{
						FVector2D TargetPositionOnScreen = FVector2D::ZeroVector;
						bool bIsTargetOnScreen = false;
						
						GetScreenPositionOfActor(*LockedOnTarget,TargetPositionOnScreen, bIsTargetOnScreen);
						
						if(IsOnScreen(TargetPositionOnScreen, StopDynamicCameraLockScreenRatio) == false)
						{
							if(bActivateDynamicCameraLock)
							{
								const FRotator InterpToTarget = CalculateInterpToTarget(*LockedOnTarget);
								PlayerController->SetControlRotation(InterpToTarget);
							}
						}
						else
						{
							bActivateDynamicCameraLock = false;
						}
					}
				}
			}
			else
			{
				// Target이 죽었을 경우 새로운 Target을 찾아 LockOn합니다.
				CancelLockOnTarget();
				ActivateLockOnTarget();

				// // 새로운 Target을 찾아 지정할 때 화면에 존재하지 않으면 LockOn을 해제합니다.
				// FVector2D TargetPositionOnScreen = FVector2D::ZeroVector;
				// bool bIsTargetOnScreen = false;
				// GetScreenPositionOfActor(*LockedOnTarget,TargetPositionOnScreen, bIsTargetOnScreen);
				// if(IsOnScreen(TargetPositionOnScreen, StopDynamicCameraLockScreenRatio) == false)
				// {
				// 	CancelLockOnTarget();
				// }
			}
		}
		else
		{
			CancelLockOnTarget();
		}
	}
}

void UPRTargetingSystemComponent::EnableCameraLock()
{
	APRAICharacter* TargetPRAICharacter = Cast<APRAICharacter>(LockedOnTarget);
	if(IsValid(TargetPRAICharacter) == true)
	{
		bActivateLockOnTarget = true;
		TargetPRAICharacter->LockedOnTarget(true);
	}
}

void UPRTargetingSystemComponent::DisableCameraLock()
{
	bActivateLockOnTarget = false;
	
	APRAICharacter* TargetPRAICharacter = Cast<APRAICharacter>(LockedOnTarget);
	if(IsValid(TargetPRAICharacter) == true)
	{
		TargetPRAICharacter->LockedOnTarget(false);
	}
}

void UPRTargetingSystemComponent::SetLockedOnTarget(AActor* NewTarget)
{
	if(IsValid(NewTarget) == true)
	{
		LockedOnTarget = NewTarget;
	}
}

void UPRTargetingSystemComponent::ChangeLockOnTargetWithMouse(const float& TurnValue)
{
	TimeSinceChangeTarget = GetWorld()->GetRealTimeSeconds() - LastTimeChangeTarget;
	if(FMath::Abs(TurnValue) > MinMouseValueToChangeTarget
		&& TimeSinceChangeTarget > MinDelayTimeToChangeTarget)
	{
		if(TurnValue < 0.0f)
		{
			SetLockedOnTarget(FindDirectionalTarget(EPRTargetDirection::TargetDirection_Left));
		}
		else
		{
			SetLockedOnTarget(FindDirectionalTarget(EPRTargetDirection::TargetDirection_Right));
		}

		LastTimeChangeTarget = GetWorld()->GetRealTimeSeconds();
	}
}

void UPRTargetingSystemComponent::ChangeLockOnTargetWithGamepad(const float& TurnRate)
{
	if(FMath::Abs(TurnRate) < MinAnalogValueToChangeTarget)
	{
		bAnalogNeutralSinceLastSwitchTarget = true;
	}

	if(FMath::Abs(TurnRate) >= MinAnalogValueToChangeTarget && bAnalogNeutralSinceLastSwitchTarget)
	{
		if(TurnRate < KINDA_SMALL_NUMBER)
		{
			SetLockedOnTarget(FindDirectionalTarget(EPRTargetDirection::TargetDirection_Left));
		}
		else if(TurnRate > KINDA_SMALL_NUMBER)
		{
			SetLockedOnTarget(FindDirectionalTarget(EPRTargetDirection::TargetDirection_Right));
		}

		bAnalogNeutralSinceLastSwitchTarget = false;
	}
}

AActor* UPRTargetingSystemComponent::FindTarget() const
{
	float DistanceFromCenterOfScreen = 0.0f;
	const FVector2D ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();
	AActor* NewTarget = nullptr;
	TArray<AActor*> TargetableActors = SearchTargetableActors();

	// 화면의 중앙에 가까운 액터를 찾아 NewTarget으로 설정합니다.
	int32 ArrayIndex = 0;
	for(AActor* TargetableActor : TargetableActors)
	{
		FVector2D ActorPositionOnScreen = FVector2D::ZeroVector;
		bool bIsActorOnScreen = false;
		GetScreenPositionOfActor(*TargetableActor, ActorPositionOnScreen, bIsActorOnScreen);
		
		const float TargetableActorPositionOnScreen = UKismetMathLibrary::Abs(ActorPositionOnScreen.X - ScreenSize.X / 2.0f);

		if(ArrayIndex == 0)
		{
			DistanceFromCenterOfScreen = TargetableActorPositionOnScreen;
		}
		else
		{
			if(TargetableActorPositionOnScreen < DistanceFromCenterOfScreen)
			{
				DistanceFromCenterOfScreen = TargetableActorPositionOnScreen;
			}
			else
			{
				continue;;
			}
		}

		NewTarget = TargetableActor;
		ArrayIndex++;
	}

	return NewTarget;
}

TArray<AActor*> UPRTargetingSystemComponent::SearchTargetableActors() const
{
	const FVector PlayerLocation = GetOwner()->GetActorLocation();
	TArray<FHitResult> HitResults;
	TArray<AActor*> TargetableActors;

	// ECollisionChannel::ECC_GameTraceChannel5는 TraceChannels의 Targeting을 나타냅니다.
	bool bIsHit = GetWorld()->SweepMultiByChannel(HitResults, PlayerLocation, PlayerLocation, FQuat::Identity,
													ECollisionChannel::ECC_GameTraceChannel5, FCollisionShape::MakeSphere(MaxSearchTargetalbeDistance));
	if(bIsHit)
	{
		for(FHitResult HitResult : HitResults)
		{
			if(HitResult.Actor.IsValid() == true)
			{
				APRAICharacter* HitedPRAICharacter = Cast<APRAICharacter>(HitResult.Actor);
				if(IsValid(HitedPRAICharacter) == true && HitedPRAICharacter->IsDead() == false)
				{
					FVector2D HitedActorPositionOnScreen = FVector2D::ZeroVector;
					bool bIsHitedActorOnScreen = false;
					GetScreenPositionOfActor(*HitedPRAICharacter,HitedActorPositionOnScreen, bIsHitedActorOnScreen);
					if(IsOnScreen(HitedActorPositionOnScreen) == true && bIsHitedActorOnScreen)
					{
						TargetableActors.AddUnique(HitResult.GetActor());
					}
				}
			}
		}
	}

	return TargetableActors;
}

AActor* UPRTargetingSystemComponent::FindDirectionalTarget(EPRTargetDirection NewTargetDirection)
{
	AActor* NewTarget = nullptr;
	TArray<AActor*> TargetableActors = SearchTargetableActors();
	const int32 CurrentTargetIndex = TargetableActors.Find(LockedOnTarget);
	
	// LockedOnTarget이 TargetableActors Array에 존재할 경우 배열에서 제거합니다.
	if(CurrentTargetIndex != INDEX_NONE)
	{
		TargetableActors.Remove(LockedOnTarget);
	}

	// LockedOnTarget을 기준으로 좌우에 존재하는 TargetableActor를 탐색합니다.
	TArray<AActor*> LeftTargetableActors;
	TArray<AActor*> RightTargetableActors;
	// LeftTargetableActors.Empty();
	// RightTargetableActors.Empty();
	for(AActor* TargetableActor : TargetableActors)
	{
		APRAICharacter* TargetablePRAICharacter = Cast<APRAICharacter>(TargetableActor);
		if(IsValid(TargetablePRAICharacter) == true && TargetablePRAICharacter->IsDead() == false)
		{
			switch(WhichSideOfTarget(TargetablePRAICharacter))
			{
			case EPRTargetDirection::TargetDirection_Left:
				LeftTargetableActors.AddUnique(TargetablePRAICharacter);
				break;
			case EPRTargetDirection::TargetDirection_Right:
				RightTargetableActors.AddUnique(TargetablePRAICharacter);
				break;
			default:
				break;
			}
		}
	}

	// // 두 벡터가 이루는 각이 클수록 두 벡터의 내적은 작습니다.
	// switch(NewTargetDirection)
	// {
	// case EPRTargetDirection::TargetDirection_Left:
	// 	if(LeftTargetableActors.Num() > 0)
	// 	{
	// 		// 왼쪽에서 내적이 가장 큰 Target
	// 		NewTarget = GetTargetByDotProduct(LeftTargetableActors, true);
	// 		
	// 		if(CurrentTargetIndex != INDEX_NONE)
	// 		{
	// 			RightTargetableActors.AddUnique(LockedOnTarget);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// 오른쪽에서 내적이 가장 작은 Target
	// 		NewTarget = GetTargetByDotProduct(RightTargetableActors, false);
	//
	// 		if(CurrentTargetIndex != INDEX_NONE)
	// 		{
	// 			LeftTargetableActors.AddUnique(LockedOnTarget);
	// 		}
	// 	}
	// 	break;
	// case EPRTargetDirection::TargetDirection_Right:
	// 	if(RightTargetableActors.Num() > 0)
	// 	{
	// 		// 오른쪽에서 내적이 가장 큰 Target
	// 		NewTarget = GetTargetByDotProduct(RightTargetableActors, true);
	// 		
	// 		if(CurrentTargetIndex != INDEX_NONE)
	// 		{
	// 			LeftTargetableActors.AddUnique(LockedOnTarget);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// 왼쪽에서 내적이 가장 작은 Target
	// 		NewTarget = GetTargetByDotProduct(LeftTargetableActors, false);
	// 		
	// 		if(CurrentTargetIndex != INDEX_NONE)
	// 		{
	// 			RightTargetableActors.AddUnique(LockedOnTarget);
	// 		}
	// 	}
	// 	break;
	// default:
	// 	break;
	// }

	// 두 벡터가 이루는 각이 클수록 두 벡터의 내적은 작습니다.
	switch(NewTargetDirection)
	{
	case EPRTargetDirection::TargetDirection_Left:
		if(LeftTargetableActors.Num() > 0)
		{
			// 왼쪽에서 내적이 가장 큰 Target
			NewTarget = GetTargetByDotProduct(LeftTargetableActors, true);
			
			if(CurrentTargetIndex != INDEX_NONE)
			{
				RightTargetableActors.AddUnique(LockedOnTarget);
			}
		}
		else
		{
			// 오른쪽에서 내적이 가장 큰 Target
			NewTarget = GetTargetByDotProduct(RightTargetableActors, true);

			if(CurrentTargetIndex != INDEX_NONE)
			{
				LeftTargetableActors.AddUnique(LockedOnTarget);
			}
		}
		break;
	case EPRTargetDirection::TargetDirection_Right:
		if(RightTargetableActors.Num() > 0)
		{
			// 오른쪽에서 내적이 가장 작은 Target
			NewTarget = GetTargetByDotProduct(RightTargetableActors, false);
			
			if(CurrentTargetIndex != INDEX_NONE)
			{
				LeftTargetableActors.AddUnique(LockedOnTarget);
			}
		}
		else
		{
			// 왼쪽에서 내적이 가장 작은 Target
			NewTarget = GetTargetByDotProduct(LeftTargetableActors, false);
			
			if(CurrentTargetIndex != INDEX_NONE)
			{
				RightTargetableActors.AddUnique(LockedOnTarget);
			}
		}
		break;
	default:
		break;
	}

	if(IsValid(NewTarget) == true)
	{
		return NewTarget;
	}

	return nullptr;
}

EPRTargetDirection UPRTargetingSystemComponent::WhichSideOfTarget(AActor* NewTargetableActor) const
{
	// APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// if(IsValid(LockedOnTarget) == true && PlayerCameraManager != nullptr)
	// {
	// 	const FVector TargetActorDirection = UKismetMathLibrary::GetDirectionUnitVector(PlayerCameraManager->GetCameraLocation(), LockedOnTarget->GetActorLocation());
	// 	const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(PlayerCameraManager->GetCameraLocation(), NewTargetableActor->GetActorLocation());
	// 	const FVector NewDirection = UKismetMathLibrary::Cross_VectorVector(TargetActorDirection, NewTargetableActorDirection);
	// 	const float NewDot = UKismetMathLibrary::Dot_VectorVector(NewDirection, GetOwner()->GetActorUpVector());
	// 	if(NewDot >= 0.0f)
	// 	{
	// 		return EPRTargetDirection::TargetDirection_Right;
	// 	}
	// }

	if(IsValid(LockedOnTarget) == true)
	{
		const float TargetDot = CalculateDotProductToTarget(*LockedOnTarget);
		const float NewTargetDot = CalculateDotProductToTarget(*NewTargetableActor);

		if(TargetDot >= NewTargetDot)
		{
			return EPRTargetDirection::TargetDirection_Left;
		}

		return EPRTargetDirection::TargetDirection_Right;
	}

	return EPRTargetDirection::TargetDirection_Left;
}

AActor* UPRTargetingSystemComponent::GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot)
{
	if(TargetableActors.Num() != 0)
	{	
		float DotProduct = 0.0f;
		AActor* NewTarget = nullptr;
		int32 ArrayIndex = 0;

		for(AActor* TargetableActor : TargetableActors)
		{
			if(ArrayIndex == 0)
			{
				DotProduct = CalculateDotProductToTarget(*TargetableActor);
				NewTarget = TargetableActor;
			}
			else
			{
				// if(bIsLargestDot)
				// {
				// 	if(CalculateDotProductToTarget(*TargetableActor) > DotProduct)
				// 	{
				// 		DotProduct = CalculateDotProductToTarget(*TargetableActor);
				// 		NewTarget = TargetableActor;
				// 	}
				// }
				// else
				// {
				// 	if(CalculateDotProductToTarget(*TargetableActor) < DotProduct)
				// 	{
				// 		DotProduct = CalculateDotProductToTarget(*TargetableActor);
				// 		NewTarget = TargetableActor;
				// 	}
				// }

				if((bIsLargestDot && CalculateDotProductToTarget(*TargetableActor) > DotProduct)
					|| (!bIsLargestDot && CalculateDotProductToTarget(*TargetableActor) < DotProduct))
				{
					DotProduct = CalculateDotProductToTarget(*TargetableActor);
					NewTarget = TargetableActor;
				}
			}

			ArrayIndex++;
		}

		return NewTarget;
	}

	return nullptr;
}

float UPRTargetingSystemComponent::CalculateDotProductToTarget(const AActor& NewTargetableActor) const
{
	// const FVector TargetActorDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), LockedOnTarget->GetActorLocation());
	// const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), NewTargetableActor.GetActorLocation());
	
	// return UKismetMathLibrary::Dot_VectorVector(TargetActorDirection, NewTargetableActorDirection);

	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(PlayerCameraManager != nullptr)
	{
		const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(PlayerCameraManager->GetCameraLocation(), NewTargetableActor.GetActorLocation());
		const FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(PlayerCameraManager->GetActorForwardVector(), NewTargetableActorDirection);

		return UKismetMathLibrary::Dot_VectorVector(PlayerCameraManager->GetActorUpVector(), CrossProduct);
	}

	return 0.0f;
}

void UPRTargetingSystemComponent::GetScreenPositionOfActor(const AActor& SearchActor, FVector2D& SearchActorPositionOnScreen, bool& bIsSearchActorOnScreen) const
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PlayerController != nullptr)
	{
		bIsSearchActorOnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, SearchActor.GetActorLocation(), SearchActorPositionOnScreen);
	}
}

bool UPRTargetingSystemComponent::IsOnScreen(const FVector2D& ActorPositionOnScreen, float ScreenRatio) const
{
	const FVector2D ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();

	// 화면 전체를 사용할 경우
	if(ScreenRatio == 1.0f || ScreenRatio == 0.0f
		|| UKismetMathLibrary::Abs(ScreenRatio) > 1.0f)
	{
		return ActorPositionOnScreen.X >= 0.0f && ActorPositionOnScreen.Y >= 0.0f
				&& ActorPositionOnScreen.X <= ScreenSize.X && ActorPositionOnScreen.Y <= ScreenSize.Y;
	}

	float LargeScreenRatio = 0.0f;
	float SmallScreenRatio = 0.0f;
	if(ScreenRatio < (1.0f - ScreenRatio))
	{
		LargeScreenRatio = 1.0f - ScreenRatio;
		SmallScreenRatio = ScreenRatio;
	}
	else
	{
		LargeScreenRatio = ScreenRatio;
		SmallScreenRatio = 1.0f - ScreenRatio;
	}
	
	return ActorPositionOnScreen.X >= ScreenSize.X * SmallScreenRatio && ActorPositionOnScreen.X <= ScreenSize.X * LargeScreenRatio
			&& ActorPositionOnScreen.Y >= ScreenSize.Y * SmallScreenRatio && ActorPositionOnScreen.Y <= ScreenSize.Y * LargeScreenRatio;
}

FRotator UPRTargetingSystemComponent::CalculateInterpToTarget(const AActor& InterpToTarget) const
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(IsValid(&InterpToTarget) == true && PlayerController != nullptr)
	{
		const FRotator PlayerControllerRotator = PlayerController->GetControlRotation();
		const FVector PlayerLocation = GetPROwner()->GetActorLocation();
		const FVector TargetLocation = InterpToTarget.GetActorLocation();

		// 플레이어와 Target의 위치를 입력하여 Target을 바라보는 방향정보를 반환합니다.
		const FRotator LookAtTarget = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);

		// 플레이어 컨트롤러를 LookAtTarget으로 회전 보간합니다.
		return FMath::RInterpTo(PlayerControllerRotator, LookAtTarget, GetWorld()->GetDeltaSeconds(), InterpSpeed);
	}

	return FRotator::ZeroRotator;
}

AActor* UPRTargetingSystemComponent::GetLockedOnTarget() const
{
	return LockedOnTarget;
}

float UPRTargetingSystemComponent::GetMaxSearchTargetalbeDistance() const
{
	return 	MaxSearchTargetalbeDistance;
}

void UPRTargetingSystemComponent::SetActivateDynamicCameraLock(bool bNewActivateDynamicCameraLock)
{
	bActivateDynamicCameraLock = bNewActivateDynamicCameraLock;
}

