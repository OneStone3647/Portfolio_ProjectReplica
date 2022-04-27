// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PRTraceCollision.h"

UPRTraceCollision::UPRTraceCollision()
{
	SetRelativeScale3D(FVector(0.1f, 2.75f, 0.1f));
	// Tick을 비활성화 합니다.
	UActorComponent::SetComponentTickEnabled(false);
	
	// TraceCollisionTop
	TraceCollisionTop = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceCollsionTop"));
	TraceCollisionTop->SetupAttachment(this);
	TraceCollisionTop->SetRelativeLocation(FVector(0.0f, 30.0f, 0.0f));
	TraceCollisionTop->SetUsingAbsoluteScale(true);									// Scale3D를 부모가 아닌 월드에 상대값으로 처리합니다.
	TraceCollisionTop->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	TraceCollisionTop->SetCollisionProfileName(TEXT("NoCollision"));
	
	// TraceCollisionBottom
	TraceCollisionBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceCollisionBottom"));
	TraceCollisionBottom->SetupAttachment(this);
	TraceCollisionBottom->SetRelativeLocation(FVector(0.0f, -30.0f, 0.0f));
	TraceCollisionBottom->SetUsingAbsoluteScale(true);								// Scale3D를 부모가 아닌 월드에 상대값으로 처리합니다.
	TraceCollisionBottom->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	TraceCollisionBottom->SetCollisionProfileName(TEXT("NoCollision"));

	// TraceCollision
	TraceCollisionStructArray.Empty();
	CreateCollisionCount = 5;
}

void UPRTraceCollision::BeginPlay()
{
	Super::BeginPlay();
	
	SetCollisionProfileName(TEXT("NoCollision"));
	GenerateTraceCollisions();
}

#pragma region Debug
void UPRTraceCollision::DrawDebugTraceCollision(bool bFlag)
{
	for(FPRTraceCollisionStruct PRTraceCollisionStruct : TraceCollisionStructArray)
	{
		PRTraceCollisionStruct.TraceCollision->SetHiddenInGame(!bFlag);
	}
}
#pragma endregion 

#pragma region TraceCollision
void UPRTraceCollision::UpdateTraceCollisionsLocation()
{
	for(int32 Index = 0; Index < TraceCollisionStructArray.Num(); Index++)
	{
		TraceCollisionStructArray[Index].PreviousLocation = TraceCollisionStructArray[Index].TraceCollision->GetComponentLocation();
	}
}

void UPRTraceCollision::GenerateTraceCollisions()
{
	// Trace에 사용되는 Box의 개수 = TraceCollisionTop(1) + TraceCollisionBottom(1) + CreateCollisionCount

	// TraceCollisionTop으로 TraceCollisionStruct를 생성하고 TraceCollisionStructArray에 저장합니다.
	FPRTraceCollisionStruct TraceCollisionTopStruct = FPRTraceCollisionStruct(TraceCollisionTop, FVector::ZeroVector);
	TraceCollisionStructArray.Add(TraceCollisionTopStruct);

	// CreateCollisionCount만큼 TraceCollisionStruct를 생성하고 TraceCollisionStructArray에 저장합니다.
	const int32 TraceCollisionStructArrayNum = TraceCollisionStructArray.Num();
	for(int32 Index = TraceCollisionStructArrayNum; Index < CreateCollisionCount + TraceCollisionStructArrayNum; Index++)
	{
		// 순서에 맞게 TraceCollision의 이름을 설정합니다.
		FString CollisionNumber = FString::FromInt(Index);
		FString Name = TEXT("TraceCollision");
		Name.Append(CollisionNumber);

		UBoxComponent* NewTraceCollision = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName(*Name));
		if(IsValid(NewTraceCollision) == true)
		{
			// NewTraceCollision을 등록하여 랜더링 / 물리적 상태로 설정합니다.
			NewTraceCollision->RegisterComponent();
			NewTraceCollision->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			NewTraceCollision->SetRelativeTransform(CalculateTraceCollisionSpawnOffsetTransform(Index));
			NewTraceCollision->SetUsingAbsoluteScale(true);								// Scale3D를 부모가 아닌 월드에 상대값으로 처리합니다.
			NewTraceCollision->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
			NewTraceCollision->SetCollisionProfileName(TEXT("NoCollision"));

			FPRTraceCollisionStruct NewTraceCollisionStruct = FPRTraceCollisionStruct(NewTraceCollision, FVector::ZeroVector);
			TraceCollisionStructArray.Add(NewTraceCollisionStruct);
		}
	}

	// TraceCollisionBottom으로 TraceCollisionStruct를 생성하고 TraceCollisionStructArray에 저장합니다.
	FPRTraceCollisionStruct TraceCollisionBottomStruct = FPRTraceCollisionStruct(TraceCollisionBottom, FVector::ZeroVector);
	TraceCollisionStructArray.Add(TraceCollisionBottomStruct);
}

float UPRTraceCollision::CalculateOffsetBetweenTraceCollisions() const
{
	// TraceCollisionTop(x), TraceCollisionBottom(y), CreateCollisionCount = 3일 경우
	// (x - y).Y / (CreateCollisionCount + 1) = OffsetBetweenTraceCollisions
	const FVector NewLocation = TraceCollisionTop->GetRelativeLocation() - TraceCollisionBottom->GetRelativeLocation();
	
	return NewLocation.Y / (CreateCollisionCount + 1);
}

FTransform UPRTraceCollision::CalculateTraceCollisionSpawnOffsetTransform(int32 TraceCollisionIndex) const
{
	const float OffsetBetweenTraceCollisions = CalculateOffsetBetweenTraceCollisions() * TraceCollisionIndex;
	FVector NewLocation = TraceCollisionBottom->GetRelativeLocation();
	NewLocation.Y += OffsetBetweenTraceCollisions;

	FTransform NewTransform = TraceCollisionBottom->GetRelativeTransform();
	NewTransform.SetLocation(NewLocation);
	NewTransform.SetScale3D(TraceCollisionBottom->GetRelativeScale3D());
	
	return NewTransform;
}
#pragma endregion 
