// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRPlayDodgeNiagaraEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRDodgeSystemComponent.h"
#include "Libraries/PREnumFunctionLibrary.h"
#include "NiagaraComponent.h"

UAN_PRPlayDodgeNiagaraEffect::UAN_PRPlayDodgeNiagaraEffect()
	: Super()
{
	DodgeNiagaraEffect = EPRDodgeNiagaraEffect::DodgeNiagaraEffect_Trail;
	LoopDuration = 0.0f;
}

void UAN_PRPlayDodgeNiagaraEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
	if(IsValid(PROwner) == true)
	{
		UNiagaraComponent* SpawnNiagaraComponent = nullptr;
		
		switch(DodgeNiagaraEffect)
		{
		case EPRDodgeNiagaraEffect::DodgeNiagaraEffect_Trail:
			SpawnNiagaraComponent = PROwner->GetDodgeSystem()->SpawnDodgeNiagaraSystem(MeshComp, PROwner->GetDodgeSystem()->GetDodgeTrailNiagaraEffect());
			break;
		case EPRDodgeNiagaraEffect::DodgeNiagaraEffect_AfterImage:
			SpawnNiagaraComponent = PROwner->GetDodgeSystem()->SpawnDodgeNiagaraSystem(MeshComp, PROwner->GetDodgeSystem()->GetDodgeAfterImageNiagaraEffect());
			break;
		default:
			break;
		}

		if(LoopDuration != 0.0f)
		{
			SpawnNiagaraComponent->SetVariableFloat("LoopDuration", LoopDuration);
		}
	}
}

FString UAN_PRPlayDodgeNiagaraEffect::GetNotifyName_Implementation() const
{
	PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	FString NewNotifyName = TEXT("PlayDodgeNiagaraEffect: ");
	NewNotifyName.Append(EnumFunctionLibrary->GetEnumDisplayNameToString(TEXT("EPRDodgeNiagaraEffect"), static_cast<uint8>(DodgeNiagaraEffect)));
	
	return NewNotifyName;
}
