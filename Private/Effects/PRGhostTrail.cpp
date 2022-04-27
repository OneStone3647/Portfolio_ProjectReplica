// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/PRGhostTrail.h"
#include "Characters/PRBaseCharacter.h"
#include "Kismet/KismetMaterialLibrary.h"

APRGhostTrail::APRGhostTrail()
{
	PrimaryActorTick.bCanEverTick = true;

	// PoseableMesh
	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
	RootComponent = PoseableMesh;

	GhostTrailColor = FLinearColor(20.000017f, 14.999998, 200.0, 1.0f);
}

void APRGhostTrail::BeginPlay()
{
	Super::BeginPlay();

	InitializeOpacityTimeline();
}

void APRGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpacityTimeline.TickTimeline(DeltaTime);
}

void APRGhostTrail::Execute(APRBaseCharacter* PROwner, FTransform Transform)
{
	SetActorHiddenInGame(false);
	SetActorTransform(Transform);
	PoseableMesh->SetSkeletalMesh(PROwner->GetMesh()->SkeletalMesh);
	
	if(FadeMaterial)
	{
		TArray<UMaterialInterface*> PoseableMeshMaterials = PoseableMesh->GetMaterials();

		for(int32 Index = 0; Index < PoseableMeshMaterials.Num(); Index++)
		{
			UMaterialInstanceDynamic* FadeDynamicMaterial = UMaterialInstanceDynamic::Create(FadeMaterial, this);
			FadeDynamicMaterial->SetVectorParameterValue(TEXT("Color"), GhostTrailColor);
			DynamicMaterials.Add(FadeDynamicMaterial);
			PoseableMesh->SetMaterial(Index, FadeDynamicMaterial);
		}
		
		PoseableMesh->CopyPoseFromSkeletalComponent(PROwner->GetMesh());
		OpacityTimeline.PlayFromStart();
	}
}

void APRGhostTrail::Disable()
{
	// if(OnOpacityIsZeroDelegate.IsBound() == true)
	// {
	// 	OnOpacityIsZeroDelegate.Broadcast(this);
	// }

	SetActorHiddenInGame(true);
}

#pragma region Opacity
void APRGhostTrail::InitializeOpacityTimeline()
{
	if(OpacityCurveFloat)
	{
		// OpacityTimeline이 사용할 Callback 함수입니다.
		FOnTimelineFloat OpacityTimelineCallback;

		// OpacityTimeline이 끝났을 때 실행할 Callback 함수입니다.
		FOnTimelineEventStatic OpacityTimelineFinishedCallback;
		
		// Callback 함수에서 사용할 함수를 바인딩합니다.
		OpacityTimelineCallback.BindUFunction(this, FName("SetOpacity"));
		OpacityTimelineFinishedCallback.BindUFunction(this, FName("Disable"));

		// Timeline에 Curve를 추가합니다.
		OpacityTimeline.AddInterpFloat(OpacityCurveFloat, OpacityTimelineCallback, NAME_None, TEXT("Opacity"));
		OpacityTimeline.SetTimelineFinishedFunc(OpacityTimelineFinishedCallback);
	}
}

void APRGhostTrail::SetOpacity(float Value)
{
	for(UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), Value);
	}
}
#pragma endregion 
