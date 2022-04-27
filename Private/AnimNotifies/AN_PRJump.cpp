// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRJump.h"
#include "Characters/PRBaseCharacter.h"
#include "Libraries/PREnumFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Kismet/GameplayStatics.h"

UAN_PRJump::UAN_PRJump()
	: Super()
{
	Jump = EPRJump::Jump_Start;
	bPlayFootStep = true;
	bPlayVoice = true;
}

void UAN_PRJump::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		if(PROwner->GetStateSystem()->IsOnDodge() == true)
		{
			return;
		}
		
		FHitResult HitResult;
		bool bIsHit = false;
		const FVector TraceStart = PROwner->GetActorLocation();
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 150.0f);
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = true;
		
		bIsHit = PROwner->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, Params);
		if(bIsHit)
		{
			PROwner->GetJumpAudio()->Play();
			PROwner->GetJumpAudio()->SetIntParameter(TEXT("SurfaceType"), static_cast<int32>(UGameplayStatics::GetSurfaceType(HitResult)));
			PROwner->GetJumpAudio()->SetIntParameter(TEXT("Jump"), static_cast<int32>(Jump));
			PROwner->GetJumpAudio()->SetBoolParameter(TEXT("PlayFootStep"), bPlayFootStep);
			PROwner->GetJumpAudio()->SetBoolParameter(TEXT("PlayVoice"), bPlayVoice);
			PROwner->GetJumpAudio()->SetFloatParameter(TEXT("FootStepVolume"), PROwner->GetFootStepVolume());
			PROwner->GetJumpAudio()->SetFloatParameter(TEXT("VoiceVolume"), PROwner->GetJumpVoiceVolume());
		}
		// DoubleJump
		else if(!bPlayFootStep)
		{
			PROwner->GetJumpAudio()->Play();
			PROwner->GetJumpAudio()->SetIntParameter(TEXT("Jump"), static_cast<int32>(Jump));
			PROwner->GetJumpAudio()->SetBoolParameter(TEXT("PlayFootStep"), bPlayFootStep);
			PROwner->GetJumpAudio()->SetBoolParameter(TEXT("PlayVoice"), bPlayVoice);
			PROwner->GetJumpAudio()->SetFloatParameter(TEXT("VoiceVolume"), PROwner->GetJumpVoiceVolume());
		}
	}
}

FString UAN_PRJump::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName;
	
	if(bPlayFootStep)
	{
		NewNotifyName = EnumFunctionLibrary->GetEnumDisplayNameToString(TEXT("EPRJump"), static_cast<uint8>(Jump));
	}
	else
	{
		NewNotifyName = TEXT("DoubleJump_Start");
	}
	
	return NewNotifyName;
}
