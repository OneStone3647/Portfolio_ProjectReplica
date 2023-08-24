// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRTimeStopSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRAnimSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UPRTimeStopSystemComponent::UPRTimeStopSystemComponent()
{
	bDebug = false;
	bActivate = false;
	bActivateTimeStopRange = true;
	TimeStopDuration = 3.0f;
	TimeStopDilation = 0.0001f;
	OwnerTimeDilation = 1.0f;
	TimeStopRange = FVector(1000.0f, 1000.0f, 1000.0f);
	BaseMotionBlurIntensity = 0.5f;
}

void UPRTimeStopSystemComponent::ActivateTimeStop()
{
	if(IsValid(GetPROwner()) == true)
	{
		bActivate = true;

		// 범위를 설정하여 TimeStop을 실행할 경우
		if(bActivateTimeStopRange)
		{
			TArray<FHitResult> HitResults;
			bool bIsHit = false;
			const FVector TraceStart = GetPROwner()->GetActorLocation();
			const FVector TraceEnd = GetPROwner()->GetActorLocation();
	
			TimeStopTarget.Empty();

			// ActorComponent의 소유자를 Trace 대상에서 제외합니다.
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Emplace(GetPROwner());

			// Debug 실행을 설정합니다.
			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
			if(bDebug)
			{
				DebugType = EDrawDebugTrace::ForDuration;
			}

			// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
			bIsHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), TraceStart, TraceEnd, TimeStopRange, FRotator::ZeroRotator,
												UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), true, ActorsToIgnore,
															DebugType, HitResults, true);
			if(bIsHit)
			{
				for(FHitResult Hit : HitResults)
				{
					if(Hit.Actor.IsValid() == true)
					{
						// 범위 안의 Actor의 CustomTimeDilation을 설정합니다.
						AActor* HitActor = Hit.GetActor();
						HitActor->CustomTimeDilation = TimeStopDilation;

						// CustomTimeDilation을 설정한 Actor를 TimeStopTarget에 저장합니다.
						TimeStopTarget.Emplace(HitActor);
					}
				}
			}
	
			// 지속시간이 끝나면 TimeStop을 종료합니다.
			GetWorld()->GetTimerManager().SetTimer(TimeStopTimerHandle, this, &UPRTimeStopSystemComponent::DeactivateTimeStop, TimeStopDuration, false);
		}
		// Owner를 제외한 월드 전체의 TimeStop을 실행할 경우
		else
		{
			// 카메라의 모션블러를 설정합니다.
			APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if(PRPlayerCharacter != nullptr)
			{
				ChangeMotionBlurAmount(PRPlayerCharacter->GetFollowCamera()->PostProcessSettings, TimeStopDilation * BaseMotionBlurIntensity);
			}
			
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeStopDilation);
			
			float NewOwnerTimeDilation = OwnerTimeDilation / TimeStopDilation;
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UPRTimeStopSystemComponent::SetOwnerCustomTimeDilation, NewOwnerTimeDilation);
			// Delay함수와 달리 타이머는 글로벌 시간 흐름 속도(Global Time Dilation)의 영향을 받습니다.
			// 이미 앞선 코드로 인해 클로벌 시간 흐름 속도가 줄어들었으므로 Delay시간을 배로 설정합니다.
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, TimeStopDilation * TimeStopDilation, false);
		}
	}
}

void UPRTimeStopSystemComponent::DeactivateTimeStop()
{
	if(IsValid(GetPROwner()) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeStopTimerHandle);
		
		bActivate = false;

		// 범위를 설정하여 TimeStop을 실행할 경우
		if(bActivateTimeStopRange)
		{
			// TimeStop의 대상이 된 Actor들의 CustomTimeDilation을 초기화합니다.
			for(AActor* Target : TimeStopTarget)
			{
				Target->CustomTimeDilation = 1.0f;
			}

			TimeStopTarget.Empty();

			// Owner의 CustomTimeDilation을 초기화합니다.
			GetPROwner()->CustomTimeDilation = 1.0f;
		}
		// Owner를 제외한 월드 전체의 TimeStop을 실행할 경우
		else
		{
			// 모션 블러의 양을 초기화합니다.
			APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if(PRPlayerCharacter != nullptr)
			{
				ChangeMotionBlurAmount(PRPlayerCharacter->GetFollowCamera()->PostProcessSettings, 0.5f);
			}
			
			// 글로벌 시간 흐름 속도를 초기화합니다.
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
			// UI사운드를 제외한 모든 사운드의 시간 흐름 속도 초기화합니다.
			UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 1.0f, 0.0f);
			// Owner의 시간 흐름 속도를 초기화합니다.
			GetPROwner()->CustomTimeDilation = 1.0f;
		}
	}
}

bool UPRTimeStopSystemComponent::IsActivateTimeStop() const
{
	return bActivate;
}

void UPRTimeStopSystemComponent::SetOwnerCustomTimeDilation(float NewCustomTimeDilation)
{
	if(IsValid(GetOwner()) == true)
	{
		GetOwner()->CustomTimeDilation = NewCustomTimeDilation;
	}
}

void UPRTimeStopSystemComponent::ChangeMotionBlurAmount(FPostProcessSettings& PostProcessSettings, float NewMotionBlurAmount)
{
	// true로 설정해야 변경된 MotionBlurAmount 값으로 설정됩니다.
	PostProcessSettings.bOverride_MotionBlurAmount = true;
	PostProcessSettings.MotionBlurAmount = NewMotionBlurAmount;
}
