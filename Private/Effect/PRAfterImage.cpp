// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PRAfterImage.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
APRAfterImage::APRAfterImage()
{
	PrimaryActorTick.bCanEverTick = true;

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PosealbeMesh"));
	RootComponent = PoseableMesh;
	DynamicMaterials.Empty();
	AfterImageMaterial = nullptr;
	AfterImageColor = FLinearColor::White;
	bActivate = false;
	FadeOutTime = 0.7f;
	FadeOutCountDown = 0.0f;

}

void APRAfterImage::BeginPlay()
{
	Super::BeginPlay();
	
}

void APRAfterImage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FadeOut(DeltaTime);
}

bool APRAfterImage::IsActivate() const
{
	return IsHidden() == false && bActivate == true;
}

void APRAfterImage::Activate(USkeletalMeshComponent* NewMesh, FTransform NewTransform)
{
	if(AfterImageMaterial != nullptr && NewMesh != nullptr && IsActivate() == false)
	{
		bActivate = true;
		FadeOutCountDown = FadeOutTime;
		SetActorHiddenInGame(false);
		SetActorTransform(NewTransform);
		PoseableMesh->SetSkeletalMesh(NewMesh->SkeletalMesh);

		// DynamicMaterials가 생성되지 않았으면 DynamicMaterials를 생성합니다.
		if(DynamicMaterials.Num() == 0)
		{
			CreatePoseableMeshMaterials(NewMesh);
		}

		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(NewMesh->GetOwner());
		if(IsValid(PROwner) == true)
		{
			AfterImageColor = PROwner->GetSignatureEffectColor();
		}
			
		for(UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), AfterImageColor);
		}

		PoseableMesh->CopyPoseFromSkeletalComponent(NewMesh);
	}
}

void APRAfterImage::CreatePoseableMeshMaterials(USkeletalMeshComponent* NewMesh)
{
	TArray<UMaterialInterface*> PoseableMeshMaterials = PoseableMesh->GetMaterials();
	for(int32 Index = 0; Index < PoseableMeshMaterials.Num(); ++Index)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(NewMesh->GetOwner());
		if(IsValid(PROwner) == true)
		{
			AfterImageColor = PROwner->GetSignatureEffectColor();
		}
			
		UMaterialInstanceDynamic* AfterImageDynamicMaterial = UMaterialInstanceDynamic::Create(AfterImageMaterial, this);
		DynamicMaterials.Add(AfterImageDynamicMaterial);
		PoseableMesh->SetMaterial(Index, AfterImageDynamicMaterial);
	}
}

void APRAfterImage::FadeOut(float DeltaTime)
{
	if(IsActivate() == true)
	{
		FadeOutCountDown -= DeltaTime;
		SetOpacity(FadeOutCountDown / FadeOutTime);
		if(FadeOutCountDown < 0.0f)
		{
			Disable();
		}
	}
}

void APRAfterImage::Disable()
{
	bActivate = false;
	SetActorHiddenInGame(true);
}

void APRAfterImage::SetOpacity(float Value)
{
	for(UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), Value);
	}
}

