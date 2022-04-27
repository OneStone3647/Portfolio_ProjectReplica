// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRFootStep.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAN_PRFootStep::UAN_PRFootStep()
	: Super()
{
	FootStepVolume = 1.0f;
}

void UAN_PRFootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		FHitResult HitResult;
		bool bIsHit = false;
		const FVector TraceStart = PROwner->GetActorLocation();
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 150.0f);
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = true;
		Params.AddIgnoredActor(MeshComp->GetOwner());		// AnimNotify를 사용하는 액터는 무시합니다.

		bIsHit = PROwner->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, Params);
		if(bIsHit)
		{
			PROwner->GetFootStepAudio()->Play();
			PROwner->GetFootStepAudio()->SetIntParameter(TEXT("SurfaceType"), static_cast<int32>(UGameplayStatics::GetSurfaceType(HitResult)));
			PROwner->GetFootStepAudio()->SetFloatParameter(TEXT("FootStepVolume"), FootStepVolume);
		}
	}
}
