// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRFootstep.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAN_PRFootstep::UAN_PRFootstep()
	: Super()
{
	// Debug
	bDebug = false;
	
	FootstepSound = nullptr;
	VolumeMultiplier = 0.6f;
	PitchMultiplier = 1.0f;
	CheckDistance = 150.0f;
	bPlayFootstep = true;
}

void UAN_PRFootstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	PlayFootstep(MeshComp);
}

void UAN_PRFootstep::PlayFootstep(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true && IsValid(FootstepSound) == true)
	{
		FHitResult HitResult;
		const FVector TraceStart = MeshComp->GetOwner()->GetActorLocation();
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, CheckDistance);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(MeshComp->GetOwner());
		
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
		}

		bool bIsHit = UKismetSystemLibrary::LineTraceSingle(MeshComp->GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
												true, ActorsToIgnore, DebugType, HitResult, true);
		if(bIsHit)
		{
			APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
			if(IsValid(PROwner) == true)
			{
				UAudioComponent* FootstepAudioComp = UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), FootstepSound, HitResult.Location);
				if(IsValid(FootstepAudioComp) == true)
				{
					FootstepAudioComp->Play();
					FootstepAudioComp->SetIntParameter(TEXT("Gender"), static_cast<int32>(PROwner->GetStatSystem()->GetGender()));
					FootstepAudioComp->SetIntParameter(TEXT("SurfaceType"), static_cast<int32>(UGameplayStatics::GetSurfaceType(HitResult)));
					FootstepAudioComp->SetFloatParameter(TEXT("VolumeMultiplier"), VolumeMultiplier);
					FootstepAudioComp->SetFloatParameter(TEXT("PitchMultiplier"), PitchMultiplier);
					FootstepAudioComp->SetBoolParameter(TEXT("PlayFootstep"), bPlayFootstep);
				}
			}
		}
	}
}
