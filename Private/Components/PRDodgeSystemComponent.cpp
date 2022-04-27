// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRDodgeSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Characters/PRPlayerCharacter.h"
#include "Enums/Enum_PRDirection.h"
#include "Effects/PRGhostTrail.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UPRDodgeSystemComponent::UPRDodgeSystemComponent()
{
	// PRAnimMontage
	CurrentDodgePRAnimMontage = FPRAnimMontage();
	ForwardDodgePRAnimMontage = FPRAnimMontage();
	BackwardDodgePRAnimMontage = FPRAnimMontage();
	LeftDodgePRAnimMontage = FPRAnimMontage();
	RightDodgePRAnimMontage = FPRAnimMontage();
	AerialDodgePRAnimMontage = FPRAnimMontage();

	// Effect
	DodgeAfterImageNiagaraEffect = nullptr;
	DodgeTrailNiagaraEffect = nullptr;
	DodgeNiagaraEffectLinearColor = FLinearColor(20.000017f, 14.999998, 200.0, 1.0f);
}

void UPRDodgeSystemComponent::ExecuteDodge(EPRDirection DodgeDirection)
{
	FVector DodgeDirectionVector = FVector::ZeroVector;

	APRPlayerCharacter* PROwnerPlayerCharacter = Cast<APRPlayerCharacter>(PROwner);
	if(PROwnerPlayerCharacter)
	{
		if(PROwnerPlayerCharacter->GetTargetingSystem()->IsLockOnTarget() == true)
		{
			const APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			const FVector PlayerCameraForwardVector = FVector(PlayerCameraManager->GetActorForwardVector().X, PlayerCameraManager->GetActorForwardVector().Y,
			                                                  0.0f).GetSafeNormal();
			const FVector PlayerCameraRightVector = FVector(PlayerCameraManager->GetActorRightVector().X, PlayerCameraManager->GetActorRightVector().Y,
			                                                0.0f).GetSafeNormal();

			switch(DodgeDirection)
			{
			case EPRDirection::Direction_Forward:
				DodgeDirectionVector = PlayerCameraForwardVector;
				break;
			case EPRDirection::Direction_ForwardLeft:
				DodgeDirectionVector = (PlayerCameraForwardVector + PlayerCameraRightVector * -1.0f).GetSafeNormal();
				break;
			case EPRDirection::Direction_ForwardRight:
				DodgeDirectionVector = (PlayerCameraForwardVector + PlayerCameraRightVector).GetSafeNormal();
				break;
			case EPRDirection::Direction_Backward:
				DodgeDirectionVector = PlayerCameraForwardVector * -1.0f;
				break;
			case EPRDirection::Direction_BackwardLeft:
				DodgeDirectionVector = (PlayerCameraForwardVector * -1.0f + PlayerCameraRightVector * -1.0f).GetSafeNormal();
				break;
			case EPRDirection::Direction_BackwardRight:
				DodgeDirectionVector = (PlayerCameraForwardVector * -1.0f + PlayerCameraRightVector).GetSafeNormal();
				break;
			case EPRDirection::Direction_Left:
				DodgeDirectionVector = PlayerCameraRightVector * -1.0f;
				break;
			case EPRDirection::Direction_Right:
				DodgeDirectionVector = PlayerCameraRightVector;
				break;
			default:
				break;
			}
			
			const FRotator LookAtTargetRotator = UKismetMathLibrary::FindLookAtRotation(PROwnerPlayerCharacter->GetActorLocation(),
																						PROwnerPlayerCharacter->GetTargetingSystem()->GetTarget()->GetActorLocation());
			PROwnerPlayerCharacter->SetActorRotation(LookAtTargetRotator);
			CurrentDodgePRAnimMontage = GetDodgePRAnimMontage(DodgeDirection);
			
		}
		else if(PROwnerPlayerCharacter->GetMovementSystem()->IsEqualMovementState(EPRMovementState::MovementState_InAir) == true)
		{
			// 공중 회피의 경우 에어리얼 상태를 적용합니다.
			DodgeDirectionVector = PROwner->GetActorForwardVector();
			PROwnerPlayerCharacter->RotateInputDirection();
			CurrentDodgePRAnimMontage = AerialDodgePRAnimMontage;
			PROwner->GetMovementSystem()->ExecuteAerial(true);
		}
		else
		{
			DodgeDirectionVector = PROwner->GetActorForwardVector();
			PROwnerPlayerCharacter->RotateInputDirection();
			CurrentDodgePRAnimMontage = ForwardDodgePRAnimMontage;
		}

	}
	else
	{
		DodgeDirectionVector = PROwner->GetActorForwardVector();
		PROwnerPlayerCharacter->RotateInputDirection();
		CurrentDodgePRAnimMontage = ForwardDodgePRAnimMontage;
	}

	PROwner->GetAnimSystem()->PlayPRAnimMontage(CurrentDodgePRAnimMontage, DodgeDirectionVector);
}

#pragma region PRAnimMontage
FPRAnimMontage UPRDodgeSystemComponent::GetDodgePRAnimMontage(EPRDirection DodgeDirection) const
{
	switch(DodgeDirection)
	{
	case EPRDirection::Direction_Forward:
		return ForwardDodgePRAnimMontage;
	case EPRDirection::Direction_Backward:
	case EPRDirection::Direction_BackwardLeft:
	case EPRDirection::Direction_BackwardRight:
		return BackwardDodgePRAnimMontage;
	case EPRDirection::Direction_Left:
	case EPRDirection::Direction_ForwardLeft:
		return LeftDodgePRAnimMontage;
	case EPRDirection::Direction_Right:
	case EPRDirection::Direction_ForwardRight:
		return RightDodgePRAnimMontage;
	default:
		return FPRAnimMontage(); 
	}
}
#pragma endregion 

#pragma region Effect
UNiagaraComponent* UPRDodgeSystemComponent::SpawnDodgeNiagaraSystem(USkeletalMeshComponent* MeshComp, UNiagaraSystem* NiagaraSystem)
{
	if(NiagaraSystem)
	{
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, MeshComp, NAME_None,
																								MeshComp->GetOwner()->GetActorLocation(), MeshComp->GetOwner()->GetActorRotation(),
																								EAttachLocation::KeepWorldPosition, false);
		SpawnNiagaraComponent->SetVariableLinearColor("LinearColor", DodgeNiagaraEffectLinearColor);

		return SpawnNiagaraComponent;
	}

	return nullptr;
}

UNiagaraSystem* UPRDodgeSystemComponent::GetDodgeAfterImageNiagaraEffect() const
{
	return DodgeAfterImageNiagaraEffect;
}

UNiagaraSystem* UPRDodgeSystemComponent::GetDodgeTrailNiagaraEffect() const
{
	return DodgeTrailNiagaraEffect;
}

FLinearColor UPRDodgeSystemComponent::GetDodgeNiagaraEffectLinearColor() const
{
	return DodgeNiagaraEffectLinearColor;
}
#pragma endregion 
