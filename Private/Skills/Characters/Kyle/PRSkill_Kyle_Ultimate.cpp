// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRSkill_Kyle_Ultimate.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Components/PRMovementSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Kismet/GameplayStatics.h"

UPRSkill_Kyle_Ultimate::UPRSkill_Kyle_Ultimate()
{
	// GeneralUltimate
	CastingGeneralUltimatePRAnimMontage = FPRAnimMontage();
	FinishGeneralUltimatePRAnimMontage = FPRAnimMontage();
	GeneralUltimateObjectName = TEXT("GeneralUltimateObject");
	GeneralUltimateCastingTime = 0.4f;
	GeneralUltimateFinishTime = 1.0f;
}

bool UPRSkill_Kyle_Ultimate::ActivateSkill_Implementation()
{
	if(IsCanActivateSkill() == true)
	{
		CastingGeneralUltimate();

		// 재사용 대기시간을 실행합니다.
		if(IsCooldown() == false)
		{
			ActivateCooldown();
		}
		
		return true;
	}
	
	return false;
}

bool UPRSkill_Kyle_Ultimate::IsCanActivateSkill_Implementation() const
{
	return Super::IsCanActivateSkill_Implementation()
			&& GetSkillOwner()->GetStateSystem()->IsActionable(EPRAction::Action_Attack);
}

void UPRSkill_Kyle_Ultimate::CastingGeneralUltimate()
{
	// 일반 공격을 초기화합니다.
	GetSkillOwner()->InitializePlayNormalAttackIndex();

	// 캐릭터를 공격하는 방향으로 회전시킵니다.
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(PRPlayerCharacter != nullptr)
	{
		if(PRPlayerCharacter->GetTargetingSystem()->IsActivateLockOnTarget() == true)
		{
			// Target이 존재하면 Target을 향해 캐릭터와 카메라를 회전합니다.
			PRPlayerCharacter->RotationAutoTargetDirection();
			PRPlayerCharacter->GetTargetingSystem()->SetActivateDynamicCameraLock(true);
		}
		else
		{
			// Target이 존재하지 않으면 카메라의 위치를 초기화하여 캐릭터의 정면을 바라봅니다.
			PRPlayerCharacter->ActivateResetCamera();
		}
	}

	// 스킬을 실행하는 동안 다른 동작을 할 수 없도록 설정합니다.
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
	GetSkillOwner()->GetStateSystem()->SetActionable(EPRAction::Action_DodgeAttack, false);
	UPRBaseSkill* DodgeAttackSkill = GetSkillOwner()->GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_DodgeAttack);
	if(IsValid(DodgeAttackSkill) == true)
	{
		DodgeAttackSkill->SetActivateSkill(false);
	}

	// 캐스팅 PRAnimMontage를 재생합니다.
	GetSkillOwner()->GetAnimSystem()->PlayPRAnimMontage(CastingGeneralUltimatePRAnimMontage);

	// 캐스팅 시간이 지난 후 GeneralUltimate를 실행합니다.
	GetWorld()->GetTimerManager().SetTimer(ActivateGeneralUltimateTimerHandle, this, &UPRSkill_Kyle_Ultimate::ActivateGeneralUltimate, GeneralUltimateCastingTime, false);
}

void UPRSkill_Kyle_Ultimate::ActivateGeneralUltimate()
{
	// 화면을 흑백으로 설정합니다.
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(PRPlayerCharacter != nullptr)
	{
		PRPlayerCharacter->ActivateWorldCameraMonochrome(true);
	}

	// 캐스팅 시간이 지난 후 GeneralUltimateObject를 활성화합니다.
	UPRObjectPoolSystemComponent* ObjectPoolSystem = GetSkillOwner()->GetObjectPoolSystem();
	if(ObjectPoolSystem != nullptr)
	{
		ObjectPoolSystem->ActivatePooledObject(GeneralUltimateObjectName);
	}

	GetWorld()->GetTimerManager().SetTimer(FinishGeneralUltimateTimerHandle, this, &UPRSkill_Kyle_Ultimate::FinishGeneralUltimate, GeneralUltimateFinishTime, false);
}

void UPRSkill_Kyle_Ultimate::FinishGeneralUltimate()
{
	// 마무리 PRAnimMontage를 재생합니다.
	GetSkillOwner()->GetAnimSystem()->PlayPRAnimMontage(FinishGeneralUltimatePRAnimMontage);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		// 화면을 원래대로 되돌립니다.
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(PRPlayerCharacter != nullptr)
		{
			PRPlayerCharacter->ActivateWorldCameraMonochrome(false);
		}
		
	}), 2.0f, false);
	
}

void UPRSkill_Kyle_Ultimate::SetSkillOwnerVisibility(bool bVisibility)
{
	
}
#pragma endregion 

#pragma region SkillInfo
void UPRSkill_Kyle_Ultimate::InitializeSkill_Implementation()
{
	Super::InitializeSkill_Implementation();
}
#pragma endregion 
