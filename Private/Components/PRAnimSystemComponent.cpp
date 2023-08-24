// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRAnimSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UPRAnimSystemComponent::UPRAnimSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// MoveLerp
	DefaultMoveLerpFloatCurve = nullptr;
	CurrentOwnerLocation = FVector::ZeroVector;

	// DataTable
	PRAnimMontageDataTable = nullptr;

	// PRAnimMontage
	CurrentPlayPRAnimMontage = FPRAnimMontage();
	CurrentPlayDirectionVector = FVector::ZeroVector;
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

#pragma region MoveLerp
void UPRAnimSystemComponent::InitializeMoveLerpTimeline()
{
	if(DefaultMoveLerpFloatCurve != nullptr)
	{
		float CurveMinTime = 0.0f;
		float CurveMaxTime = 0.0f;

		if(MoveLerpTimelineProgress.IsBound() == false)
		{
			// Callback 함수에서 사용할 함수를 바인딩합니다.
			MoveLerpTimelineProgress.BindUFunction(this, FName("MoveLerp"));
			MoveLerpTimelineFinish.BindUFunction(this, FName("MoveLerpFinish"));

			// Timeline에 Curve를 추가합니다.
			MoveLerpTimeline.AddInterpFloat(DefaultMoveLerpFloatCurve, MoveLerpTimelineProgress, NAME_None, TEXT("MoveLerp"));
			// Timeline에 Finish함수를 추가합니다.
			MoveLerpTimeline.SetTimelineFinishedFunc(MoveLerpTimelineFinish);
			DefaultMoveLerpFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
			MoveLerpTimeline.SetPlayRate(1.0f);
		}
		else
		{
			// Timeline을 초기값으로 초기화합니다.
			MoveLerpTimeline.SetFloatCurve(DefaultMoveLerpFloatCurve, TEXT("MoveLerp"));
			DefaultMoveLerpFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
			MoveLerpTimeline.SetPlayRate(1.0f);
		}
		
		MoveLerpTimeline.SetTimelineLength(CurveMaxTime);
	}
}

void UPRAnimSystemComponent::MoveLerp(float Value)
{
	if(CurrentPlayPRAnimMontage.MoveLerpFloatCurve != nullptr)
	{
		float CurveMinValue = 0.0f;
		float CurveMaxValue = 0.0f;
		float Alpha = 0.0f;
		
		CurrentPlayPRAnimMontage.MoveLerpFloatCurve->GetValueRange(CurveMinValue, CurveMaxValue);
		if(CurveMaxValue >= 0.0f)
		{
			Alpha = FMath::Clamp(Value / CurveMaxValue, 0.0f, 1.0f);
		}
		else
		{
			Alpha = FMath::Clamp(Value / CurveMinValue, 0.0f, 1.0f);
		}
		
		const FVector MoveLocation = FMath::Lerp(CurrentOwnerLocation, CurrentPlayDirectionVector * CurrentPlayPRAnimMontage.MoveDistance + CurrentOwnerLocation, Alpha);
		const FVector NewMoveLocation = FVector(MoveLocation.X, MoveLocation.Y, GetPROwner()->GetActorLocation().Z);
		
		GetPROwner()->SetActorLocation(NewMoveLocation, true);
	}
}

void UPRAnimSystemComponent::MoveLerpFinish()
{
	if(GetPROwner()->GetStateSystem()->IsActionable(EPRAction::Action_Move) == false)
	{
		// GetPROwner()->GetStateSystem()->SetMoveable(true);
		GetPROwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, true);
	}
}

FTimeline& UPRAnimSystemComponent::GetMoveLerpTimeline()
{
	return MoveLerpTimeline;
}

#pragma endregion

#pragma region PRAnimMontage
bool UPRAnimSystemComponent::IsPlayPRAnimMontage() const
{
	if(GetPROwner() == nullptr)
	{
		return false;
	}
	
	UAnimInstance* PROwnerAnimInstance = GetPROwner()->GetMesh()->GetAnimInstance();

	return MoveLerpTimeline.IsPlaying() == true
			|| PROwnerAnimInstance->IsAnyMontagePlaying() == true;
}

void UPRAnimSystemComponent::PlayPRAnimMontage(FPRAnimMontage NewPRAnimMontage, FVector PlayDirectionVector)
{
	if(GetPROwner() != nullptr)
	{
		CurrentPlayPRAnimMontage = NewPRAnimMontage;
		CurrentOwnerLocation = GetPROwner()->GetActorLocation();
		// GetPROwner()->GetStateSystem()->SetMoveable(false);
		GetPROwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);

		// 방향벡터 설정
		if(PlayDirectionVector == FVector::ZeroVector)
		{
			// 재생하는 방향벡터가 영벡터일 경우 캐릭터의 전방벡터를 사용합니다.
			CurrentPlayDirectionVector = GetPROwner()->GetActorForwardVector();
		}
		else
		{
			// 재생하는 방향벡터가 영벡터가 아닐 경우 재생하는 방향벡터를 정규화하여 사용합니다.
			CurrentPlayDirectionVector = PlayDirectionVector.GetSafeNormal();
		}

		float CurveMinTime = 0.0f;
		float CurveMaxTime = 0.0f;
		
		// Timeline의 Curve 설정
		if(CurrentPlayPRAnimMontage.MoveLerpFloatCurve != nullptr)
		{
			// CurrentPRAnimMontage의 MoveLerpFloatCurve가 존재할 경우 MoveLerpTimeline에 설정된 MoveLerp FloatTrack의 FloatCurve를
			// CurrentPrAnimMontage의 MoveLerpFloatCurve로 설정합니다.
			MoveLerpTimeline.SetFloatCurve(CurrentPlayPRAnimMontage.MoveLerpFloatCurve, TEXT("MoveLerp"));
			// 타임라인의 재생속도를 변경하여 캐릭터의 이동속도에 변화를 줍니다.
			MoveLerpTimeline.SetPlayRate(CurrentPlayPRAnimMontage.MoveSpeedRatio);
			CurrentPlayPRAnimMontage.MoveLerpFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
		}
		else
		{
			if(DefaultMoveLerpFloatCurve != nullptr)
			{
				// CurrentPRAnimMontage의 MoveLerpFloatCurve가 존재하지 앓은 경우 MoveLerpTimeline에 설정된 MoveLerp FloatTrack의 FloatCurve를
				// DefaultMoveLerpFloatCurve로 설정합니다.
				MoveLerpTimeline.SetFloatCurve(DefaultMoveLerpFloatCurve, TEXT("MoveLerp"));
				MoveLerpTimeline.SetPlayRate(1.0f);
				DefaultMoveLerpFloatCurve->GetTimeRange(CurveMinTime, CurveMaxTime);
				CurrentPlayPRAnimMontage.MoveLerpFloatCurve = DefaultMoveLerpFloatCurve;
			}
		}
		
		GetPROwner()->PlayAnimMontage(NewPRAnimMontage.AnimMontage, NewPRAnimMontage.PlayRate);
		MoveLerpTimeline.SetTimelineLength(CurveMaxTime);
		MoveLerpTimeline.PlayFromStart();
	}
}

void UPRAnimSystemComponent::StopPRAnimMontage()
{
	if(GetPROwner()->GetStateSystem()->IsCanCancelAction() == false)
	{
		return;
	}
	
	MoveLerpTimeline.Stop();
	GetPROwner()->StopAnimMontage();
	GetPROwner()->GetMovementSystem()->ActivateAerial(false);
}

FPRAnimMontage UPRAnimSystemComponent::GetPRAnimMontageFromDataTableByID(UDataTable* NewDataTable, const int32 ID)
{
	if(NewDataTable != nullptr)
	{
		FString NewStringID = FString::FromInt(ID);
		FName NewNameID = FName(*NewStringID);
		FPRAnimMontage* DataTableRow = NewDataTable->FindRow<FPRAnimMontage>(NewNameID, FString(""));
		if(DataTableRow != nullptr)
		{
			return *DataTableRow;
		}
	}

	return FPRAnimMontage();
}

FPRAnimMontage UPRAnimSystemComponent::GetPRAnimMontageFromPRAnimMontageDataTableByID(const int32 ID)
{
	return GetPRAnimMontageFromDataTableByID(PRAnimMontageDataTable, ID);
}

TArray<FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromDataTableToArray(UDataTable* NewDataTable)
{
	TArray<FPRAnimMontage> NewPRAnimMontageArray;
	if(NewDataTable != nullptr)
	{
		TArray<FName> RowNames = NewDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRAnimMontage* DataTableRow = NewDataTable->FindRow<FPRAnimMontage>(RowName, FString(""));
			if(DataTableRow != nullptr)
			{
				NewPRAnimMontageArray.Emplace(*DataTableRow);
			}
		}
	}

	return NewPRAnimMontageArray;
}

TArray<FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromDataTableByIDRangeToArray(UDataTable* NewDataTable, const int32 FromID, const int32 ToID)
{
	TArray<FPRAnimMontage> NewPRAnimMontageArray;
	if(NewDataTable != nullptr)
	{
		for(int32 Index = FromID; Index <= ToID; Index++)
		{
			FString NewStringID = FString::FromInt(Index);
			FName NewNameID = FName(*NewStringID);
			FPRAnimMontage* DataTableRow = NewDataTable->FindRow<FPRAnimMontage>(NewNameID, FString(""));
			if(DataTableRow != nullptr)
			{
				NewPRAnimMontageArray.Emplace(*DataTableRow);
			}
		}
	}

	return NewPRAnimMontageArray;
}

TArray<FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToArray(const int32 FromID, const int32 ToID)
{
	return GetPRAnimMontageFromDataTableByIDRangeToArray(PRAnimMontageDataTable, FromID, ToID);
}

// TMap<int32, FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromDataTableToMap(UDataTable* NewDataTable)
// {
// 	TMap<int32, FPRAnimMontage> NewPRAnimMontages;
// 	if(NewDataTable != nullptr)
// 	{
// 		TArray<FName> RowNames = NewDataTable->GetRowNames();
// 		for(const FName& RowName : RowNames)
// 		{
// 			FPRAnimMontage DataTableRow = *NewDataTable->FindRow<FPRAnimMontage>(RowName, FString(""));
// 			NewPRAnimMontages.Emplace(RowName, DataTableRow);
// 		}
// 	}
//
// 	return NewPRAnimMontages;
// }

TMap<FName, FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromDataTableToMap(UDataTable* NewDataTable)
{
	TMap<FName, FPRAnimMontage> NewPRAnimMontageMap;
	if(NewDataTable)
	{
		TArray<FName> RowNames = NewDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRAnimMontage* DataTableRow = NewDataTable->FindRow<FPRAnimMontage>(RowName, FString(""));
			if(DataTableRow != nullptr)
			{
				NewPRAnimMontageMap.Emplace(RowName, *DataTableRow);
			}
		}
	}

	return NewPRAnimMontageMap;
}

TMap<FName, FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromDataTableByIDRangeToMap(UDataTable* NewDataTable, const int32 FromID, const int32 ToID)
{
	TMap<FName, FPRAnimMontage> NewPRAnimMontageMap;
	if(NewDataTable)
	{
		TArray<FName> RowNames = NewDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRAnimMontage* DataTableRow = NewDataTable->FindRow<FPRAnimMontage>(RowName, FString(""));
			if(DataTableRow != nullptr)
			{
				NewPRAnimMontageMap.Emplace(RowName, *DataTableRow);
			}
		}
	}

	return NewPRAnimMontageMap;
}

TMap<FName, FPRAnimMontage> UPRAnimSystemComponent::GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToMap(const int32 FromID, const int32 ToID)
{
	return GetPRAnimMontageFromDataTableByIDRangeToMap(PRAnimMontageDataTable, FromID, ToID);
}

FPRAnimMontage UPRAnimSystemComponent::GetCurrentPlayPRAnimMontage() const
{
	return CurrentPlayPRAnimMontage;
}
#pragma endregion 

#pragma region PRAttackAnimMontage
// void UPRAnimSystemComponent::InitializePRAttackAnimMontageIndex(FPRAttackAnimMontage& NewPRAttackAnimMontage)
// {
// 	NewPRAttackAnimMontage.InitializePlayPRAnimMontageIndex();
// }
//
// void UPRAnimSystemComponent::IncreasePRAttackAnimMontageIndex(FPRAttackAnimMontage& NewPRAttackAnimMontage)
// {
// 	NewPRAttackAnimMontage.IncreasePlayPRAnimMontageIndex();
// }
#pragma endregion

