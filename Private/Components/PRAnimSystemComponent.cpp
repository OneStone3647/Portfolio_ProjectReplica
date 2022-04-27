// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRAnimSystemComponent.h"
#include "Characters/PRBaseCharacter.h"

UPRAnimSystemComponent::UPRAnimSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// MoveLerp
	DefaultMoveLerpCurveFloat = nullptr;
	OwnerCurrentLocation = FVector::ZeroVector;
}

void UPRAnimSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMoveLerpTimeline();
}

void UPRAnimSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveLerpTimeline.TickTimeline(DeltaTime);
}

void UPRAnimSystemComponent::InitializeMoveLerpTimeline()
{
	if(DefaultMoveLerpCurveFloat)
	{
		// Curve가 사용할 Callback 함수입니다.
		FOnTimelineFloat MoveLerpTimelineCallback;

		// Callback 함수에서 사용할 함수를 바인딩합니다.
		MoveLerpTimelineCallback.BindUFunction(this, FName("MoveLerp"));

		// Timeline에 Curve를 추가합니다.
		MoveLerpTimeline.AddInterpFloat(DefaultMoveLerpCurveFloat, MoveLerpTimelineCallback, NAME_None, TEXT("MoveLerp"));
	}
}

void UPRAnimSystemComponent::MoveLerp(float Value)
{
	const float Alpha = FMath::Clamp(Value * CurrentPRAnimMontage.MoveSpeedRatio, 0.0f, 1.0f);
	const FVector MoveLocation = FMath::Lerp(OwnerCurrentLocation, DirectionVector * CurrentPRAnimMontage.MoveDistance + OwnerCurrentLocation,
												Alpha);

	PROwner->SetActorLocation(MoveLocation, true);
}

void UPRAnimSystemComponent::PlayPRAnimMontage(FPRAnimMontage NewPRAnimMontage, FVector PlayDirectionVector)
{
	CurrentPRAnimMontage = NewPRAnimMontage;
	OwnerCurrentLocation = PROwner->GetActorLocation();
	PROwner->PlayAnimMontage(CurrentPRAnimMontage.AnimMontage, CurrentPRAnimMontage.PlayRate);

	if(PlayDirectionVector == FVector::ZeroVector)
	{
		// 재생 방향벡터가 영벡터일 경우 캐릭터의 전방벡터를 사용합니다.
		DirectionVector = PROwner->GetActorForwardVector();
	}
	else
	{
		// 재생 방향벡터가 영벡터가 아닐 경우 정규화합니다.
		DirectionVector = PlayDirectionVector.GetSafeNormal();
	}

	if(CurrentPRAnimMontage.MoveLerpCurveFloat)
	{
		// CurrentPRAnimMontage의 MoveLerpCurveFloat가 존재할 경우 MoveLerpTimeline에 설정된 MoveLerp FloatTrack의 FloatCurve를
		// CurrentPRAnimMontage의 MoveLerpCurveFloat로 설정합니다.
		MoveLerpTimeline.SetFloatCurve(CurrentPRAnimMontage.MoveLerpCurveFloat, TEXT("MoveLerp"));
	}
	else
	{
		// CurrentPRAnimMontage의 MoveLerpCurveFloat가 존재할 경우 MoveLerpTimeline에 설정된 MoveLerp FloatTrack의 FloatCurve를
		// DefaultMoveLerpCurveFloat로 설정합니다.
		MoveLerpTimeline.SetFloatCurve(DefaultMoveLerpCurveFloat, TEXT("MoveLerp"));
	}

	MoveLerpTimeline.PlayFromStart();
}

void UPRAnimSystemComponent::StopPRAnimMontage()
{
	PROwner->StopAnimMontage();
	if(MoveLerpTimeline.IsPlaying() == true)
	{
		MoveLerpTimeline.Stop();
	}
}

#pragma region AnimMontage
UAnimMontage* UPRAnimSystemComponent::GetHitAnimMontage() const
{
	return HitAnimMontage;
}
#pragma endregion 
