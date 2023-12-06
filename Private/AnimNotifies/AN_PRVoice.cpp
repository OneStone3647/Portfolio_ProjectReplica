// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRVoice.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAN_PRVoice::UAN_PRVoice()
	: Super()
{
	Voice = nullptr;
	VolumeMultiplier = 1.5f;
	PitchMultiplier = 1.0f;
	bPlayVoice = true;
}

void UAN_PRVoice::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	PlayVoice(MeshComp);
}

FString UAN_PRVoice::GetNotifyName_Implementation() const
{
	if(IsValid(Voice) == true)
	{
		FString NewNotifyName;
		NewNotifyName.Append(TEXT("PlayVoice: "));
		NewNotifyName.Append(Voice->GetName());
		
		return NewNotifyName;
	}
	
	return Super::GetNotifyName_Implementation();
}

void UAN_PRVoice::PlayVoice(USkeletalMeshComponent* MeshComp)
{
	if(IsValid(MeshComp->GetOwner()) == true && IsValid(Voice) == true)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) == true)
		{
			UAudioComponent* VoiceAudioComp = UGameplayStatics::SpawnSoundAttached(Voice, MeshComp);
			if(VoiceAudioComp != nullptr)
			{
				VoiceAudioComp->Play();
				VoiceAudioComp->SetIntParameter(TEXT("Gender"), static_cast<int32>(PROwner->GetStatSystem()->GetGender()));
				VoiceAudioComp->SetFloatParameter(TEXT("VolumeMultiplier"), VolumeMultiplier);
				VoiceAudioComp->SetFloatParameter(TEXT("PitchMultiplier"), PitchMultiplier);
				VoiceAudioComp->SetBoolParameter(TEXT("PlayVoice"), bPlayVoice);
			}
		}
	}
}
